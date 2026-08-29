using Proxirae.Contracts.Data.Rules.Actions;

namespace Proxirae.Contracts.Data.Rules
{
    public class RuleData
    {
        public Guid Id { get; init; }
        public int Priority { get; set; }
        public bool IsEnabled { get; init; }
        public string? Remarks { get; init; }
        public string? Processes { get; init; }
        public string? Hosts { get; init; }
        public string? Ports { get; init; }
        public RuleDataProtocol Protocol { get; init; }
        public ActionData Action { get; init; } = null!;
    }
}
