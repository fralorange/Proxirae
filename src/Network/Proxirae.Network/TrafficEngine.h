#pragma once

#include <windivert.h>
#include <vector>

namespace Proxirae {
	namespace Network {
		class TrafficEngine {
		public:
			TrafficEngine();
			~TrafficEngine();

			void Start();
			void Stop();

		private:
			HANDLE m_handle;
			std::vector<unsigned char> m_packetBuffer;
			UINT m_recvLen;
			WINDIVERT_ADDRESS m_addr;
			bool m_running;
		};
	}
}
