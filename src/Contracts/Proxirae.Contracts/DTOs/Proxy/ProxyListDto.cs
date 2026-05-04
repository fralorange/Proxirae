namespace Proxirae.Contracts.DTOs.Proxy
{
    public class ProxyListDto
    {
        public Guid Id { get; set; }
        public string? Remarks { get; set; }
        public required string IP { get; set; }
        public int Port { get; set; }
        public ProxyTypeDto Type { get; set; }
    }
}
