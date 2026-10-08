using System.Buffers.Binary;

namespace Proxirae.Presentation.WPF.Models.Endpoint
{
    public record Endpoint
    {
        public string Address { get; }
        public ushort Port { get; }

        public Endpoint(string address, ushort networkPort)
        {
            Address = address;
            Port = BinaryPrimitives.ReverseEndianness(networkPort);
        }

        public override string ToString() => $"{Address}:{Port}";
    }
}
