#include "ipc/IpcMessenger.h"

namespace Proxirae {
	IpcMessenger::IpcMessenger(ISendChannel& sender)
		: m_sender(sender) { }
}