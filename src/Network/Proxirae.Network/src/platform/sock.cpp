#include "platform/sock.h"

#ifdef _WIN32

namespace Proxirae {
	void CloseSocket(NativeSocket s) { closesocket(static_cast<SOCKET>(s)); }
	int GetSocketError() { return WSAGetLastError(); }
}

#elif defined(__linux__)

namespace Proxirae {
	void CloseSocket(NativeSocket s) { close(s); }
	int GetSocketError() { return errno; }
}
#else 
#error Unsupported platform
#endif 