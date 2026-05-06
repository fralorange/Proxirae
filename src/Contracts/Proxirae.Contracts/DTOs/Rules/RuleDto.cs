using Proxirae.Contracts.DTOs.Rules.Actions;

namespace Proxirae.Contracts.DTOs.Rules
{
    public class RuleDto
    {
        public Guid Id { get; set; }
        public bool IsEnabled { get; set; } = true;
        public string? Remarks { get; set; }
        public string? Processes { get; set; } 
        public string? Hosts { get; set; } 
        public string? Ports { get; set; } 
        public RuleProtocolDto Protocol { get; set; }
        public BaseActionDto Action { get; set; } = new DirectActionDto();
    }
}
