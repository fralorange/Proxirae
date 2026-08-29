using Proxirae.Domain.Rules.Actions;

namespace Proxirae.Domain.Rules
{
    public class Rule
    {
        public Guid Id { get; set; }
        public int Priority { get; set; }
        public bool IsEnabled { get; set; } = true;
        public string? Remarks { get; set; }
        public string? Processes { get; set; } 
        public string? Hosts { get; set; } 
        public string? Ports { get; set; } 
        public RuleProtocol Protocol { get; set; }
        public BaseAction Action { get; set; } = new DirectAction();
    }
}
