#include <windivert.h>


int main() {
    HANDLE h = WinDivertOpen("tcp.DstPort == 80", WINDIVERT_LAYER_NETWORK, 0, 0);
}