using System.Runtime.InteropServices;

namespace Proxirae.Infrastructure.ProcessCommunication.Pipes.Message
{
    public class PipeMessage
    {
        public PipeMessageHeader Header { get; set; }
        public byte[] Payload { get; set; } = [];

        public static PipeMessage Create(ushort type)
        {
            return new PipeMessage
            {
                Header = new PipeMessageHeader
                {
                    Type = type,
                }
            };
        }

        public static PipeMessage Create(ushort type, byte[] payload)
        {
            return new PipeMessage
            {
                Header = new PipeMessageHeader
                {
                    Type = type,
                },
                Payload = payload
            };
        }

        public static bool TryParse(ReadOnlySpan<byte> rawData, out PipeMessage? message)
        {
            message = null;
            int headerSize = Marshal.SizeOf<PipeMessageHeader>();

            if (rawData.Length < headerSize)
            {
                return false;
            }

            var header = MemoryMarshal.Read<PipeMessageHeader>(rawData[..headerSize]);

            if (header.Magic != 0x5850)
            {
                return false;
            }

            message = new PipeMessage { Header = header };

            if (header.PayloadSize > 0)
            {
                if (rawData.Length < headerSize + header.PayloadSize)
                {
                    return false;
                }

                message.Payload = rawData.Slice(headerSize, (int)header.PayloadSize).ToArray();
            }

            return true;
        }
    }
}
