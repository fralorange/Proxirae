namespace Proxirae.Contracts.DTOs.Routes
{
    public class RouteDto
    {
        public DateTime Timestamp { get; set; }
        public long ProcessId { get; set; }
        public uint Address { get; set; }
        public ushort Port { get; set; }
        public Guid RuleId { get; set; }
    }
}
