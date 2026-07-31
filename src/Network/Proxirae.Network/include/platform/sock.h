#pragma once

#include "platform/sock_types.h"

#ifdef _WIN32
	#include <WinSock2.h>
	#include <WS2tcpip.h>

	namespace Proxirae {
		constexpr int SocketError = SOCKET_ERROR;
		constexpr int SocketNotConnected = WSAENOTCONN;
		constexpr int ShutdownBoth = SD_BOTH;
		using SocketLen = int;

		inline void CloseSocket(NativeSocket s) { closesocket(static_cast<SOCKET>(s)); }
		inline int GetSocketError() { return WSAGetLastError(); }
	}

#elif defined(__linux__)
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <unistd.h>
	#include <cerrno>

	namespace Proxirae {
		constexpr int SocketError = -1;
		constexpr int SocketNotConnected = ENOTCONN;
		constexpr int ShutdownBoth = SHUT_RDWR;
		using SocketLen = socklen_t;

		inline void CloseSocket(NativeSocket s) { close(s); }
		inline int GetSocketError() { return errno; }
	}
#else 
	#error Unsupported platform
#endif 
