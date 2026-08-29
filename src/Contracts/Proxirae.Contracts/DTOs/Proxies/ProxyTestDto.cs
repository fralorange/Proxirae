namespace Proxirae.Contracts.DTOs.Proxies
{
    public class ProxyTestDto
    {
        public string Address { get; set; } = string.Empty;
        public int Port { get; set; }
        public ProxyTypeDto Type { get; set; }
        public string? Username { get; set; }
        public string? Password { get; set; }
    }
}
