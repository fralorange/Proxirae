namespace Proxirae.Domain.Rules.Actions
{
    public class BlockAction : BaseAction
    {
        public override Guid Id => KnownActionIds.Block;
        public override string Name => "Block";
    }
}
