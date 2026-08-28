namespace Proxirae.Contracts.DTOs.Proxies
{
    public class ProxyListDto
    {
        public Guid Id { get; set; }
        public string? Remarks { get; set; }
        public required string Address { get; set; }
        public int Port { get; set; }
        public ProxyTypeDto Type { get; set; }
    }
}
