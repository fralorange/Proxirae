namespace Proxirae.Contracts.DTOs.Rules.Actions
{
    public abstract class BaseActionDto
    {
        public Guid Id { get; init; }
        public abstract string Name { get; }
    }
}
