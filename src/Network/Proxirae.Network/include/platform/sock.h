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

		void CloseSocket(NativeSocket s);
		int GetSocketError();
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

		void CloseSocket(NativeSocket s);
		int GetSocketError();
	}
#else 
	#error Unsupported platform
#endif 
