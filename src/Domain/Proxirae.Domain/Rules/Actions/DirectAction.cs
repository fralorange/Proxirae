namespace Proxirae.Domain.Rules.Actions
{
    public class DirectAction : BaseAction
    {
        public override Guid Id => KnownActionIds.Direct;
        public override string Name => "Direct";
    }
}
