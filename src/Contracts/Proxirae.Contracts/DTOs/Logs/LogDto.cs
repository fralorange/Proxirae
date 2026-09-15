namespace Proxirae.Contracts.DTOs.Logs
{
    public class LogDto
    {
        public DateTime Timestamp { get; set; }
        public LogLevelDto Level { get; set; }
        public string Message { get; set; } = null!;
    }
}
