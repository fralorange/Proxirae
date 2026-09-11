#include <stdexcept>
#include <format>

#include "environment/sock.h"
#include "environment/inet.h"
#include "proxification/datagram/UdpSocks5Proxy.h"

namespace Proxirae {

    UdpSocks5Proxy::UdpSocks5Proxy(std::string_view address, std::uint16_t port, IAsyncDriver& driver, IIoDatagramAdapter& adapter, ILogger& logger)
        : Socks5ProxyBase(address, port, driver, logger), m_adapter(adapter)
    {
        m_internalRecvBuffer.resize(65536);
    }

    UdpSocks5Proxy::UdpSocks5Proxy(std::string_view address, std::uint16_t port, std::string_view username, std::string_view password, IAsyncDriver& driver, IIoDatagramAdapter& adapter, ILogger& logger)
        : Socks5ProxyBase(address, port, username, password, driver, logger), m_adapter(adapter)
    {
        m_internalRecvBuffer.resize(65536);
    }

    UdpSocks5Proxy::~UdpSocks5Proxy() {
        Disconnect();
    }

    bool UdpSocks5Proxy::Connect() {
        if (m_connected) {
            m_logger.LogWarning(std::format("[SOCKS5] Connect skipped: already connected to {}:{}", m_address, m_port));
            return false;
        }

        NativeSocket proxySocket = ConnectToProxy();
        if (proxySocket == InvalidNativeSocket) {
            return false;
        }

        if (!PerformHandshake(proxySocket)) {
            CloseSocket(proxySocket);
            return false;
        }

        if (!RequestUdpAssociate(proxySocket)) {
            CloseSocket(proxySocket);
            return false;
        }

        NativeSocket relaySocket = ConnectToRelay(proxySocket);
        if (relaySocket == InvalidNativeSocket) {
            return false;
        }

        m_tcpControl = proxySocket;
        m_udpData = relaySocket;

        m_connected = true;
        m_logger.LogInfo(std::format("[SOCKS5] Associate tunnel established at {}:{}", m_bindAddress, m_bindPort));

        return true;
    }

    void UdpSocks5Proxy::Disconnect() {
        if (!m_connected) return;

        shutdown(m_tcpControl, ShutdownBoth);
        CloseSocket(m_tcpControl);

        CloseSocket(m_udpData);

        m_tcpControl = InvalidNativeSocket;
        m_udpData = InvalidNativeSocket;
        m_connected = false;

        m_logger.LogInfo(std::format("[SOCKS5] Disconnected from {}:{}", m_address, m_port));
    }

    void UdpSocks5Proxy::Send(
        std::span<const std::byte> buffer,
        std::string_view targetAddress,
        std::uint16_t targetPort,
        std::function<void(const IoResult&)> callback)
    {
        if (!m_connected) {
            callback(IoResult{ false, 0, SocketNotConnected });
            return;
        }

        auto packet = std::make_shared<std::vector<std::byte>>();
        packet->reserve(262 + buffer.size());

        packet->push_back(std::byte{ 0x00 }); // RSV
        packet->push_back(std::byte{ 0x00 }); // RSV
        packet->push_back(std::byte{ 0x00 }); // FRAG 

        std::string targetStr(targetAddress);
        struct in_addr ipv4Addr {};
        struct in6_addr ipv6Addr {};

        if (inet_pton(AF_INET, targetStr.c_str(), &ipv4Addr) == 1) {
            packet->push_back(std::byte{ 0x01 }); // IPv4
            const auto* ipBytes = reinterpret_cast<const std::byte*>(&ipv4Addr.s_addr);
            packet->insert(packet->end(), ipBytes, ipBytes + 4);
        }
        else if (inet_pton(AF_INET6, targetStr.c_str(), &ipv6Addr) == 1) {
            packet->push_back(std::byte{ 0x04 }); // IPv6
            const auto* ipBytes = reinterpret_cast<const std::byte*>(&ipv6Addr.s6_addr);
            packet->insert(packet->end(), ipBytes, ipBytes + 16);
        }
        else {
            packet->push_back(std::byte{ 0x03 }); // Domain
            packet->push_back(static_cast<std::byte>(targetStr.length()));
            const auto* strBytes = reinterpret_cast<const std::byte*>(targetStr.data());
            packet->insert(packet->end(), strBytes, strBytes + targetStr.length());
        }

        packet->push_back(static_cast<std::byte>((targetPort >> 8) & 0xFF));
        packet->push_back(static_cast<std::byte>(targetPort & 0xFF));

        packet->insert(packet->end(), buffer.begin(), buffer.end());

        struct sockaddr_in relayAddr {};
        relayAddr.sin_family = AF_INET;
        inet_pton(AF_INET, m_bindAddress.c_str(), &relayAddr.sin_addr.s_addr);
        relayAddr.sin_port = htons(m_bindPort);

        m_adapter.AsyncSendTo(
            m_udpData,
            reinterpret_cast<const sockaddr*>(&relayAddr),
            sizeof(relayAddr),
            *packet,
            [packet, callback = std::move(callback)](const IoDatagramResult& res) {
                callback(res);
            }
        );
    }

    void UdpSocks5Proxy::Recv(
        std::span<std::byte> buffer,
        std::function<void(const IoResult& result, std::string sourceAddress, std::uint16_t sourcePort)> callback)
    {
        if (!m_connected) {
            callback(IoResult{ false, 0, SocketNotConnected }, "", 0);
            return;
        }

        m_adapter.AsyncRecvFrom(m_udpData, m_internalRecvBuffer, [this, buffer, callback = std::move(callback)](const IoDatagramResult& res) {
            if (!res.success || res.bytesTransferred < 10) {
                callback(res, "", 0);
                return;
            }

            std::size_t offset = 0;
            auto* data = m_internalRecvBuffer.data();

            offset += 3; // RSV + RSV + FRAG

            std::byte atyp = data[offset++];
            std::string sourceAddr;
            std::size_t addrLen = 0;

            if (atyp == std::byte{ 0x01 }) { // IPv4
                char ipStr[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &data[offset], ipStr, sizeof(ipStr));
                sourceAddr = ipStr;
                addrLen = 4;
            }
            else if (atyp == std::byte{ 0x03 }) { // Domain
                std::size_t domainLen = static_cast<std::size_t>(data[offset]);
                offset++;
                sourceAddr = std::string(reinterpret_cast<const char*>(&data[offset]), domainLen);
                addrLen = domainLen;
            }
            else if (atyp == std::byte{ 0x04 }) { // IPv6
                char ipStr[INET6_ADDRSTRLEN];
                inet_ntop(AF_INET6, &data[offset], ipStr, sizeof(ipStr));
                sourceAddr = ipStr;
                addrLen = 16;
            }
            else {
                m_logger.LogError("[SOCKS5] Unknown address type in received packet");
                callback(IoResult{ false, 0, 0 }, "", 0);
                return;
            }

            offset += addrLen;

            std::uint16_t sourcePort = (static_cast<std::uint16_t>(data[offset]) << 8) |
                static_cast<std::uint16_t>(data[offset + 1]);
            offset += 2;

            std::size_t payloadSize = res.bytesTransferred - offset;

            if (payloadSize > buffer.size()) {
                m_logger.LogWarning("[SOCKS5] Dropped packet: user buffer too small");
                callback(IoResult{ false, 0, 0 }, "", 0);
                return;
            }

            std::copy_n(data + offset, payloadSize, buffer.data());

            callback(IoResult{ true, payloadSize, res.errorCode }, sourceAddr, sourcePort);
            });
    }

    bool UdpSocks5Proxy::RequestUdpAssociate(NativeSocket sock)
    {
        std::vector<char> req{
            0x05, 0x03, 0x00,
            0x01, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00
        };

        if (!SendExact(sock, req)) return false;

        char header[4]{};
        if (!RecvExact(sock, header)) return false;

        if (header[1] != 0x00) {
            m_logger.LogError(std::format("[SOCKS5] Associate request rejected: status {}", static_cast<int>(header[1])));
            return false;
        }

        if (header[3] == 0x01) { // IPv4
            char ipRaw[4]{};
            RecvExact(sock, ipRaw);
            char ipStr[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, ipRaw, ipStr, sizeof(ipStr));
            m_bindAddress = ipStr;
        }
        else if (header[3] == 0x04) { // IPv6
            char ipRaw[16]{};
            RecvExact(sock, ipRaw);
            char ipStr[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, ipRaw, ipStr, sizeof(ipStr));
            m_bindAddress = ipStr;
        }
        else if (header[3] == 0x03) { // Domain
            char len = 0;
            RecvExact(sock, std::span(&len, 1));
            std::vector<char> domain(static_cast<unsigned char>(len));
            RecvExact(sock, domain);
            m_bindAddress = std::string(domain.begin(), domain.end());
        }

        char portRaw[2]{};
        RecvExact(sock, portRaw);
        m_bindPort = (static_cast<unsigned char>(portRaw[0]) << 8) | static_cast<unsigned char>(portRaw[1]);

        if (m_bindAddress == "0.0.0.0" || m_bindAddress == "::") {
            m_bindAddress = m_address;
        }

        return true;
    }

    NativeSocket UdpSocks5Proxy::ConnectToRelay(NativeSocket sock)
    {
        NativeSocket relaySocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (relaySocket == InvalidNativeSocket) {
            CloseSocket(sock);
            return InvalidNativeSocket; 
        }

        struct sockaddr_in bindAddr {};
        inet_pton(AF_INET, m_bindAddress.c_str(), &bindAddr.sin_addr.s_addr);
        bindAddr.sin_port = htons(m_bindPort);
        bindAddr.sin_family = AF_INET;

        if (connect(relaySocket, reinterpret_cast<struct sockaddr*>(&bindAddr), sizeof(bindAddr)) == SocketError) {
            m_logger.LogError("[SOCKS5] Failed to connect local UDP socket to proxy bind address.");
            CloseSocket(sock);
            CloseSocket(relaySocket);
            return InvalidNativeSocket;
        }

        if (!m_driver.Attach(relaySocket)) {
            CloseSocket(sock);
            CloseSocket(relaySocket);
            return InvalidNativeSocket;
        }

        return relaySocket;
    }
}