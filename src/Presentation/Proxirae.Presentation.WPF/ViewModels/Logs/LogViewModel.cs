using Proxirae.Contracts.DTOs.Logs;

namespace Proxirae.Presentation.WPF.ViewModels.Logs
{
    public class LogViewModel
    {
        public LogLevelDto Level { get; set; }
        public string Message { get; set; } = null!;
        public DateTime Timestamp { get; set; }

        public LogViewModel(LogDto log)
        {
            Level = log.Level;
            Message = log.Message;
            Timestamp = log.Timestamp;
        }

        public override string ToString()
        {
            return $"{Timestamp:HH::mm::ss} {Level} {Message}";
        }
    }
}
