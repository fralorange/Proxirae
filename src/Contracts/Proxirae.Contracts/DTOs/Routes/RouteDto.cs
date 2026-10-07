namespace Proxirae.Contracts.DTOs.Routes
{
    public class RouteDto
    {
        public DateTime Timestamp { get; set; }
        public long ProcessId { get; set; }
        public string Address { get; set; } = null!;
        public ushort Port { get; set; }
        public Guid RuleId { get; set; }
    }
}
