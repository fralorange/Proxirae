namespace Proxirae.Application.Facades.Configuration
{
    public interface IConfigurationFacade
    {
        string AppDataDirectory { get; }
        string[] ConfigurationFiles { get; }

        Task ReloadAsync(CancellationToken cancellationToken);
    }
}
