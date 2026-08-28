using System.Runtime.InteropServices;

namespace Proxirae.Infrastructure.ProcessCommunication.Pipes.Message
{
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct PipeMessageHeader
    {
        public ushort Magic;
        public ushort Type;
        public uint Id;
        public uint PayloadSize;
    }
}
