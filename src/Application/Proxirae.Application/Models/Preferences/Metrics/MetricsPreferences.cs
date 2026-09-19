namespace Proxirae.Application.Models.Preferences.Metrics
{
    public record MetricsPreferences
    {
        public int LogsBufferSize { get; init; } = 1000;
        public int RoutingBufferSize { get; init; } = 1000;
    }
}
