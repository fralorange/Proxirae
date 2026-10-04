#include <format>
#include <cstring>
#include <memory>

#include "environment/sock.h"
#include "environment/inet.h"
#include "proxification/datagram/UdpHttpsProxy.h"

namespace Proxirae {

    UdpHttpsProxy::UdpHttpsProxy(std::string_view address, std::uint16_t port, IAsyncDriver& driver, IIoStreamAdapter& adapter, ILogger& logger)
        : HttpsProxyBase(address, port, driver, logger), m_streamAdapter(adapter) {
        m_readCtx = std::make_shared<FrameReadContext>();
        m_readCtx->headerBuffer.resize(2); 
    }

    UdpHttpsProxy::UdpHttpsProxy(std::string_view address, std::uint16_t port, std::string_view username, std::string_view password, IAsyncDriver& driver, IIoStreamAdapter& adapter, ILogger& logger)
        : HttpsProxyBase(address, port, username, password, driver, logger), m_streamAdapter(adapter) {
        m_readCtx = std::make_shared<FrameReadContext>();
        m_readCtx->headerBuffer.resize(2);
    }

    UdpHttpsProxy::~UdpHttpsProxy() {
        Disconnect();
    }

    bool UdpHttpsProxy::Connect() {
        if (m_connected) return false;

        NativeSocket sock = ConnectToProxy();
        if (sock == InvalidNativeSocket) return false;

        std::string request =
            "CONNECT udp-relay.internal:0 HTTP/1.1\r\n"
            "Host: udp-relay.internal:0\r\n"
            "Proxy-Connection: Keep-Alive\r\n"
            "X-UDP-Forward: 1\r\n"; 

        if (!m_username.empty() && !m_password.empty()) {
            std::string credentials = std::format("{}:{}", m_username, m_password);
            request += std::format("Proxy-Authorization: Basic {}\r\n", Base64Encode(credentials));
        }

        request += "\r\n";

        if (!SendExact(sock, std::span<const char>(request.data(), request.size()))) {
            m_logger.LogError(std::format("[HTTPS] CONNECT request failed to send ({}:{})", m_address, m_port));
            CloseSocket(sock);
            return false;
        }

        std::string headers;
        if (!ReadHttpHeaders(sock, headers)) {
            CloseSocket(sock);
            return false;
        }

        if (headers.find(" 200") == std::string::npos || headers.find("X-UDP-Forward") == std::string::npos) {
            m_logger.LogWarning("[HTTPS] UDP encapsulation not supported by this proxy.");
            CloseSocket(sock);
            return false; 
        }

        if (!m_driver.Attach(sock)) {
            CloseSocket(sock);
            return false;
        }

        m_tcpProxySocket = sock;
        m_connected = true;
        m_logger.LogInfo(std::format("[HTTPS] UDP encapsulation tunnel established via {}:{}", m_address, m_port));

        return true;
    }

    void UdpHttpsProxy::Disconnect() {
        if (!m_connected) return;

        shutdown(m_tcpProxySocket, ShutdownBoth);
        CloseSocket(m_tcpProxySocket);

        m_tcpProxySocket = InvalidNativeSocket;
        m_connected = false;
        m_logger.LogInfo(std::format("[HTTPS] Disconnected UDP tunnel from {}:{}", m_address, m_port));
    }

    void UdpHttpsProxy::Send(
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

        if (isIpv4) { atyp = std::byte{ 0x01 }; addrLen = 4; }
        else if (isIpv6) { atyp = std::byte{ 0x04 }; addrLen = 16; }
        else { atyp = std::byte{ 0x03 }; addrLen = 1 + targetAddress.length(); }

        std::size_t payloadSize = addrLen + 3 + buffer.size(); // ATYP(1) + ADDR + PORT(2) + DATA
        std::uint16_t networkPayloadSize = htons(static_cast<std::uint16_t>(payloadSize));

        auto sendBuffer = std::make_shared<std::vector<std::byte>>();
        sendBuffer->reserve(2 + payloadSize);

        const auto* sizeBytes = reinterpret_cast<const std::byte*>(&networkPayloadSize);
        sendBuffer->insert(sendBuffer->end(), sizeBytes, sizeBytes + 2);

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

        m_streamAdapter.AsyncWrite(m_tcpProxySocket, *sendBuffer, [sendBuffer, callback = std::move(callback)](const IoResult& res) {
            callback(res);
        });
    }

    void UdpHttpsProxy::Recv(
        std::span<std::byte> buffer,
        std::function<void(const IoResult& result, std::string sourceAddress, std::uint16_t sourcePort)> callback)
    {
        if (!m_connected) {
            callback(IoResult{ false, 0, SocketNotConnected }, "", 0);
            return;
        }

        m_streamAdapter.AsyncRead(m_tcpProxySocket, m_readCtx->headerBuffer,
            [this, buffer, callback = std::move(callback)](const IoResult& lenRes) mutable {

                if (!lenRes.success || lenRes.bytesTransferred < 2) {
                    callback(IoResult{ false, 0, 0 }, "", 0);
                    return;
                }

                std::uint16_t payloadLen;
                std::memcpy(&payloadLen, m_readCtx->headerBuffer.data(), 2);
                m_readCtx->payloadLength = ntohs(payloadLen);

                if (m_readCtx->payloadLength == 0 || m_readCtx->payloadLength > 65535) {
                    callback(IoResult{ false, 0, 0 }, "", 0);
                    return;
                }

                m_readCtx->payloadBuffer.resize(m_readCtx->payloadLength);

                m_streamAdapter.AsyncRead(m_tcpProxySocket, m_readCtx->payloadBuffer,
                    [this, buffer, callback = std::move(callback)](const IoResult& dataRes) {

                        if (!dataRes.success || dataRes.bytesTransferred < m_readCtx->payloadLength) {
                            callback(IoResult{ false, 0, 0 }, "", 0);
                            return;
                        }

                        const auto* data = m_readCtx->payloadBuffer.data();
                        std::size_t offset = 0;
                        std::byte atyp = data[offset++];
                        std::size_t addrLen = 0;
                        std::string sourceAddr;

                        if (atyp == std::byte{ 0x01 }) {
                            addrLen = 4;
                            char ip[INET_ADDRSTRLEN];
                            inet_ntop(AF_INET, &data[offset], ip, sizeof(ip));
                            sourceAddr = ip;
                            offset += addrLen;
                        }
                        else if (atyp == std::byte{ 0x03 }) {
                            addrLen = static_cast<std::size_t>(data[offset]);
                            offset++;
                            sourceAddr = std::string(reinterpret_cast<const char*>(&data[offset]), addrLen);
                            offset += addrLen;
                        }
                        else if (atyp == std::byte{ 0x04 }) {
                            addrLen = 16;
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

                        std::size_t actualPayloadSize = m_readCtx->payloadLength - offset;
                        std::size_t toCopy = (std::min)(actualPayloadSize, buffer.size());
                        std::memcpy(buffer.data(), &data[offset], toCopy);

                        callback(IoResult{ true, toCopy, 0 }, sourceAddr, sourcePort);
                    }
                );
            }
        );
    }
}