using System.Buffers.Binary;
using System.Net;

namespace Proxirae.Presentation.WPF.Models.Endpoint
{
    public record Endpoint
    {
        public string Address { get; }
        public ushort Port { get; }

        public Endpoint(uint networkAddress, ushort networkPort) 
        {
            Address = new IPAddress(BitConverter.GetBytes(networkAddress)).ToString();
            Port = BinaryPrimitives.ReverseEndianness(networkPort);
        }

        public override string ToString() => $"{Address}:{Port}";
    }
}
