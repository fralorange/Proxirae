namespace Proxirae.Contracts.DTOs.Logs
{
    public class LogDto
    {
        public LogLevelDto Level { get; set; }
        public string Message { get; set; } = null!;
        public DateTime Timestamp { get; set; }
    }
}
