#include <stdexcept>
#include <format>

#include "platform/environment/sock.h"
#include "platform/environment/inet.h"
#include "features/proxification/datagram/UdpSocks5Proxy.h"

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

        m_tcpReceiver = proxySocket;
        m_udpRelay = relaySocket;

        m_connected = true;
        m_logger.LogInfo(std::format("[SOCKS5] Associate tunnel established at {}:{}", m_bindAddress, m_bindPort));

        return true;
    }

    void UdpSocks5Proxy::Disconnect() {
        if (!m_connected) return;

        shutdown(m_tcpReceiver, ShutdownBoth);
        CloseSocket(m_tcpReceiver);
        CloseSocket(m_udpRelay);

        m_tcpReceiver = InvalidNativeSocket;
        m_udpRelay = InvalidNativeSocket;
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

        std::size_t headerSize = addrLen + 6;

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

        m_adapter.AsyncSendTo(
            m_udpRelay,
            reinterpret_cast<const sockaddr*>(&m_relaySockAddr),
            m_relaySockAddrLen,
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

        m_adapter.AsyncRecvFrom(m_udpRelay, m_internalRecvBuffer, [this, buffer, callback = std::move(callback)](const IoDatagramResult& res) {
            if (!res.success || res.bytesTransferred < 4) {
                callback(res, "", 0);
                return;
            }

            const auto* data = m_internalRecvBuffer.data();

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
        struct sockaddr_storage localAddr {};
        NativeSocketLen addrLen = sizeof(localAddr);

        if (getsockname(sock, reinterpret_cast<struct sockaddr*>(&localAddr), &addrLen) == SocketError) {
            m_logger.LogError(std::format("[SOCKS5] Failed to get sock name: error {}", GetSocketError()));
            return false;
        }

        std::vector<char> assReq;

        if (localAddr.ss_family == AF_INET6) {
            assReq = { 0x05, 0x03, 0x00, 0x04 };
            assReq.insert(assReq.end(), 18, 0x00);
        }
        else {
            assReq = { 0x05, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
        }

        if (!SendExact(sock, assReq)) return false;

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
        struct addrinfo hints {};
        std::memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_DGRAM; 
        hints.ai_protocol = IPPROTO_UDP;

        std::string portStr = std::to_string(m_bindPort);
        struct addrinfo* result = nullptr;

        int res = getaddrinfo(m_bindAddress.c_str(), portStr.c_str(), &hints, &result);
        if (res != 0 || result == nullptr) {
            m_logger.LogError(std::format("[SOCKS5] Failed to resolve relay address {}: error {}", m_bindAddress, res));
            CloseSocket(sock);
            return InvalidNativeSocket;
        }

        NativeSocket relaySocket = InvalidNativeSocket;
        std::memset(&m_relaySockAddr, 0, sizeof(m_relaySockAddr));
        m_relaySockAddrLen = 0;

        for (struct addrinfo* ptr = result; ptr != nullptr; ptr = ptr->ai_next) {
            relaySocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
            if (relaySocket == InvalidNativeSocket) {
                continue;
            }

            if (connect(relaySocket, ptr->ai_addr, static_cast<NativeSocketLen>(ptr->ai_addrlen)) != SocketError) {
                std::memcpy(&m_relaySockAddr, ptr->ai_addr, ptr->ai_addrlen);
                m_relaySockAddrLen = static_cast<NativeSocketLen>(ptr->ai_addrlen);
                break;
            }

            CloseSocket(relaySocket);
            relaySocket = InvalidNativeSocket;
        }

        freeaddrinfo(result);

        if (relaySocket == InvalidNativeSocket) {
            m_logger.LogError(std::format("[SOCKS5] Failed to connect local UDP socket to proxy bind address {}:{}", m_bindAddress, m_bindPort));
            CloseSocket(sock);
            return InvalidNativeSocket;
        }

        if (!m_driver.Attach(relaySocket)) {
            m_logger.LogError("[SOCKS5] Failed to attach relay socket to driver.");
            CloseSocket(relaySocket);
            return InvalidNativeSocket;
        }

        return relaySocket;
    }
}