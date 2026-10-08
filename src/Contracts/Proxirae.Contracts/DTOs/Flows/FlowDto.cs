namespace Proxirae.Contracts.DTOs.Flows
{
    public class FlowDto
    {
        public Guid Id { get; set; }
        public string TargetAddress { get; set; } = null!;
        public ushort TargetPort { get; set; }
        public long ProcessId { get; set; }
        public ulong SecondsPassed { get; set; }
        public Guid ProxyId { get; set; }
        public ulong BytesSent { get; set; }
        public ulong BytesReceived { get; set; }
        public FlowStatusDto Status { get; set; }
    }
}
