#include <format>
#include "environment/sock.h"
#include "proxification/stream/TcpHttpsProxy.h"

namespace Proxirae {

    TcpHttpsProxy::TcpHttpsProxy(std::string_view address, std::uint16_t port, IAsyncDriver& driver, IIoStreamAdapter& adapter, ILogger& logger)
        : HttpsProxyBase(address, port, driver, logger), m_adapter(adapter) {
    }

    TcpHttpsProxy::TcpHttpsProxy(std::string_view address, std::uint16_t port, std::string_view username, std::string_view password, IAsyncDriver& driver, IIoStreamAdapter& adapter, ILogger& logger)
        : HttpsProxyBase(address, port, username, password, driver, logger), m_adapter(adapter) {
    }

    TcpHttpsProxy::~TcpHttpsProxy() {
        Disconnect();
    }

    bool TcpHttpsProxy::Connect(std::string_view targetAddress, std::uint16_t targetPort) {
        if (m_connected) {
            m_logger.LogWarning(std::format("[HTTPS] Connect skipped: already connected to {}:{}", m_address, m_port));
            return false;
        }

        NativeSocket localSocket = ConnectToProxy();
        if (localSocket == InvalidNativeSocket) return false;

        if (!ConnectToTarget(localSocket, targetAddress, targetPort)) {
            CloseSocket(localSocket);
            return false;
        }

        if (!m_driver.Attach(localSocket)) {
            CloseSocket(localSocket);
            return false;
        }

        m_proxy = localSocket;
        m_connected = true;
        return true;
    }

    void TcpHttpsProxy::Disconnect() {
        if (!m_connected) return;

        m_logger.LogDebug(std::format("[HTTPS] Closing connection to {}:{}", m_address, m_port));
        shutdown(m_proxy, ShutdownBoth);
        CloseSocket(m_proxy);

        m_proxy = InvalidNativeSocket;
        m_connected = false;
        m_logger.LogInfo(std::format("[HTTPS] Disconnected from {}:{}", m_address, m_port));
    }

    void TcpHttpsProxy::Send(std::span<const std::byte> buffer, std::function<void(const IoResult&)> callback) {
        if (!m_connected || m_proxy == InvalidNativeSocket) {
            callback(IoResult{ false, 0, SocketNotConnected });
            return;
        }
        m_adapter.AsyncWrite(m_proxy, buffer, callback);
    }

    void TcpHttpsProxy::Recv(std::span<std::byte> buffer, std::function<void(const IoResult&)> callback) {
        if (!m_connected || m_proxy == InvalidNativeSocket) {
            callback(IoResult{ false, 0, SocketNotConnected });
            return;
        }
        m_adapter.AsyncRead(m_proxy, buffer, callback);
    }

    bool TcpHttpsProxy::ConnectToTarget(NativeSocket sock, std::string_view targetAddress, std::uint16_t targetPort) {
        std::string request = std::format(
            "CONNECT {}:{} HTTP/1.1\r\n"
            "Host: {}:{}\r\n"
            "Proxy-Connection: Keep-Alive\r\n",
            targetAddress, targetPort, targetAddress, targetPort
        );

        if (!m_username.empty() && !m_password.empty()) {
            std::string credentials = std::format("{}:{}", m_username, m_password);
            request += std::format("Proxy-Authorization: Basic {}\r\n", Base64Encode(credentials));
        }

        request += "\r\n"; 

        if (!SendExact(sock, std::span<const char>(request.data(), request.size()))) {
            m_logger.LogError(std::format("[HTTPS] CONNECT request failed to send ({}:{})", m_address, m_port));
            return false;
        }

        std::string headers;
        if (!ReadHttpHeaders(sock, headers)) {
            return false;
        }

        std::size_t firstLineEnd = headers.find("\r\n");
        std::string_view firstLine = (firstLineEnd != std::string::npos)
            ? std::string_view(headers.data(), firstLineEnd)
            : headers;

        if (firstLine.find(" 200") == std::string_view::npos) {
            m_logger.LogError(std::format("[HTTPS] Proxy rejected CONNECT: {}", firstLine));
            return false;
        }

        m_logger.LogInfo(std::format("[HTTPS] Tunnel established to {}:{} via {}:{}", targetAddress, targetPort, m_address, m_port));
        return true;
    }
}