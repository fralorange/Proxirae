namespace Proxirae.Contracts.DTOs.Proxy
{
    public class ProxyDetailDto
    {
        public Guid Id { get; set; }
        public string? Remarks { get; set; }
        public string IP { get; set; } = string.Empty;
        public int Port { get; set; }
        public ProxyTypeDto Type { get; set; }
        public string? Username { get; set; }
        public string? Password { get; set; }
    }
}
