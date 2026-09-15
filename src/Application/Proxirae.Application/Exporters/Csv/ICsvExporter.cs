namespace Proxirae.Application.Exporters.Csv
{
    public interface ICsvExporter
    {
        Task ExportAsync<T>(string filePath, IEnumerable<T> items, CancellationToken cancellationToken);
    }
}
