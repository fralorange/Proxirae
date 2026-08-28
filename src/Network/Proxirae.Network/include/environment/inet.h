#pragma once

#ifdef _WIN32
	#include <WinSock2.h>
	#include <WS2tcpip.h>
#elif defined(__linux__)
	#include <arpa/inet.h>
	#include <netinet/in.h>
#else
	#error Unsupported platform
#endif