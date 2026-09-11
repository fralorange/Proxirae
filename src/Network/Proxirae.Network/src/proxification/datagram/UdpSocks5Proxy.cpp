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

        std::size_t addrLen = 0;
        std::byte atyp;
        struct in_addr addr4 {};
        struct in6_addr addr6 {};

        std::string addressStr(targetAddress);

        bool isIpv4 = (inet_pton(AF_INET, addressStr.c_str(), &addr4) == 1);
        bool isIpv6 = !isIpv4 && (inet_pton(AF_INET6, addressStr.c_str(), &addr6) == 1);

        if (isIpv4) {
            atyp = std::byte{ 0x01 };
            addrLen = 4;
        }
        else if (isIpv6) {
            atyp = std::byte{ 0x04 };
            addrLen = 16;
        }
        else {
            atyp = std::byte{ 0x03 };
            addrLen = 1 + targetAddress.length();
        }

        std::size_t headerSize = 3 + 1 + addrLen + 2;

        auto sendBuffer = std::make_shared<std::vector<std::byte>>();
        sendBuffer->reserve(headerSize + buffer.size());

        sendBuffer->push_back(std::byte{ 0x00 }); // RSV
        sendBuffer->push_back(std::byte{ 0x00 }); // RSV
        sendBuffer->push_back(std::byte{ 0x00 }); // FRAG
        sendBuffer->push_back(atyp);

        if (isIpv4) {
            const auto* bytes = reinterpret_cast<const std::byte*>(&addr4.s_addr);
            sendBuffer->insert(sendBuffer->end(), bytes, bytes + 4);
        }
        else if (isIpv6) {
            const auto* bytes = reinterpret_cast<const std::byte*>(&addr6.s6_addr);
            sendBuffer->insert(sendBuffer->end(), bytes, bytes + 16);
        }
        else {
            sendBuffer->push_back(static_cast<std::byte>(targetAddress.length()));
            const auto* bytes = reinterpret_cast<const std::byte*>(targetAddress.data());
            sendBuffer->insert(sendBuffer->end(), bytes, bytes + targetAddress.length());
        }

        std::uint16_t networkPort = htons(targetPort);
        const auto* portBytes = reinterpret_cast<const std::byte*>(&networkPort);
        sendBuffer->insert(sendBuffer->end(), portBytes, portBytes + 2);

        sendBuffer->insert(sendBuffer->end(), buffer.begin(), buffer.end());

        struct sockaddr_in relayAddr {};
        relayAddr.sin_family = AF_INET;
        inet_pton(AF_INET, m_bindAddress.c_str(), &relayAddr.sin_addr.s_addr);
        relayAddr.sin_port = htons(m_bindPort);

        m_adapter.AsyncSendTo(
            m_udpData,
            reinterpret_cast<const sockaddr*>(&relayAddr),
            sizeof(relayAddr),
            *sendBuffer,
            [sendBuffer, callback = std::move(callback)](const IoDatagramResult& res) {
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

        auto recvBuffer = std::make_shared<std::vector<std::byte>>(65536);

        m_adapter.AsyncRecvFrom(m_udpData, *recvBuffer, [this, buffer, recvBuffer, callback = std::move(callback)](const IoDatagramResult& res) {
            if (!res.success || res.bytesTransferred < 10) {
                callback(res, "", 0);
                return;
            }

            const auto* data = recvBuffer->data();

            if (res.bytesTransferred < 4) {
                callback(IoResult{ false, 0, 0 }, "", 0);
                return;
            }

            if (data[2] != std::byte{ 0x00 }) {
                callback(IoResult{ false, 0, 0 }, "", 0);
                return;
            }

            std::size_t offset = 3;
            std::byte atyp = data[offset++];
            std::size_t addrLen = 0;
            std::string sourceAddr;

            if (atyp == std::byte{ 0x01 }) { // IPv4
                addrLen = 4;
                if (res.bytesTransferred < offset + addrLen + 2) {
                    callback(IoResult{ false, 0, 0 }, "", 0);
                    return;
                }

                char ip[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &data[offset], ip, sizeof(ip));
                sourceAddr = ip;
                offset += addrLen;
            }
            else if (atyp == std::byte{ 0x03 }) { // Domain Name
                if (res.bytesTransferred < offset + 1) {
                    callback(IoResult{ false, 0, 0 }, "", 0);
                    return;
                }

                std::size_t domainLen = static_cast<std::size_t>(data[offset]);
                offset++;
                addrLen = domainLen;

                if (res.bytesTransferred < offset + addrLen + 2) {
                    callback(IoResult{ false, 0, 0 }, "", 0);
                    return;
                }

                sourceAddr = std::string(reinterpret_cast<const char*>(&data[offset]), addrLen);
                offset += addrLen;
            }
            else if (atyp == std::byte{ 0x04 }) { // IPv6
                addrLen = 16;
                if (res.bytesTransferred < offset + addrLen + 2) {
                    callback(IoResult{ false, 0, 0 }, "", 0);
                    return;
                }

                char ip[INET6_ADDRSTRLEN];
                inet_ntop(AF_INET6, &data[offset], ip, sizeof(ip));
                sourceAddr = ip;
                offset += addrLen;
            }
            else {
                callback(IoResult{ false, 0, 0 }, "", 0);
                return;
            }

            std::uint16_t sourcePort;
            std::memcpy(&sourcePort, &data[offset], sizeof(sourcePort));
            sourcePort = ntohs(sourcePort);
            offset += 2;

            std::size_t payloadSize = res.bytesTransferred - offset;
            std::size_t toCopy = (std::min)(payloadSize, buffer.size());

            std::memcpy(buffer.data(), &data[offset], toCopy);

            callback(IoResult{ true, toCopy, 0 }, sourceAddr, sourcePort);
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
        struct sockaddr_storage bindAddr {};
        socklen_t addrLen = 0;
        int domain = AF_INET;

        if (inet_pton(AF_INET, m_bindAddress.c_str(), &reinterpret_cast<struct sockaddr_in*>(&bindAddr)->sin_addr) == 1) {
            domain = AF_INET;
            auto* addr4 = reinterpret_cast<struct sockaddr_in*>(&bindAddr);
            addr4->sin_family = AF_INET;
            addr4->sin_port = htons(m_bindPort);
            addrLen = sizeof(struct sockaddr_in);
        }
        else if (inet_pton(AF_INET6, m_bindAddress.c_str(), &reinterpret_cast<struct sockaddr_in6*>(&bindAddr)->sin6_addr) == 1) {
            domain = AF_INET6;
            auto* addr6 = reinterpret_cast<struct sockaddr_in6*>(&bindAddr);
            addr6->sin6_family = AF_INET6;
            addr6->sin6_port = htons(m_bindPort);
            addrLen = sizeof(struct sockaddr_in6);
        }
        else {
            m_logger.LogError(std::format("[SOCKS5] Invalid relay address: {}", m_bindAddress));
            CloseSocket(sock);
            return InvalidNativeSocket;
        }

        NativeSocket relaySocket = socket(domain, SOCK_DGRAM, IPPROTO_UDP);
        if (relaySocket == InvalidNativeSocket) {
            CloseSocket(sock);
            return InvalidNativeSocket;
        }

        if (connect(relaySocket, reinterpret_cast<struct sockaddr*>(&bindAddr), addrLen) == SocketError) {
            m_logger.LogError("[SOCKS5] Failed to connect local UDP socket to proxy bind address.");
            CloseSocket(sock);
            CloseSocket(relaySocket);
            return InvalidNativeSocket;
        }

        if (!m_driver.Attach(relaySocket)) {
            m_logger.LogError("[SOCKS5] Failed to attach relay socket to driver.");
            CloseSocket(sock);
            CloseSocket(relaySocket);
            return InvalidNativeSocket;
        }

        return relaySocket;
    }
}