namespace Proxirae.Application.Services.Package
{
    public interface IPackageService
    {
        Task<bool?> ExportAsync(
            string archivePath,
            Func<CancellationToken, Task<string?>> masterPasswordProvider,
            CancellationToken token = default);

        Task<bool> ImportAsync(
            string archivePath,
            Func<CancellationToken, Task<string?>> masterPasswordProvider,
            CancellationToken token = default);
    }
}
