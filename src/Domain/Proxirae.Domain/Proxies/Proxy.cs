namespace Proxirae.Domain.Proxies
{
    public class Proxy
    {
        public Guid Id { get; init; }
        public string? Remarks { get; set; }
        public required string IP { get; set; }
        public int Port { get; set; }
        public ProxyType Type { get; set; }
        public string? Username { get; set; }
        public string? Password { get; set; }
    }
}
