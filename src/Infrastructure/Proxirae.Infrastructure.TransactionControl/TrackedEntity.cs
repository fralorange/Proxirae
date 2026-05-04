namespace Proxirae.Infrastructure.TransactionControl
{
    internal class TrackedEntity<T> where T : class
    {
        public required T Entity { get; set; }
        public EntityState State { get; set; }
    }
}
