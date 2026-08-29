namespace Proxirae.Domain.Rules.Actions
{
    public abstract class BaseAction
    {
        public abstract Guid Id { get; }
        public abstract string Name { get; }
    }
}
