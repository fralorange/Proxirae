namespace Proxirae.Contracts.DTOs.Test
{
    public class TestProgressDto
    {
        public Guid Id { get; set; }
        public TestProgressStage Stage { get; set; }
        public ulong? Latency { get; set; }
    }
}
