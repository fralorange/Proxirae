using nietras.SeparatedValues;
using Proxirae.Application.Exporters.Csv;
using System.Globalization;
using System.Reflection;

namespace Proxirae.Infrastructure.Exporters.Csv
{
    internal class SepCsvExporter : ICsvExporter
    {
        public async Task ExportAsync<T>(string filePath, IEnumerable<T> items, CancellationToken cancellationToken = default)
        {
            await Task.Run(() =>
            {
                var properties = typeof(T).GetProperties(BindingFlags.Public | BindingFlags.Instance);

                using var writer = Sep.Writer().ToFile(filePath);

                foreach (var item in items)
                {
                    cancellationToken.ThrowIfCancellationRequested();

                    using var row = writer.NewRow();
                    foreach (var prop in properties)
                    {
                        var value = prop.GetValue(item);

                        string formattedValue = value switch
                        {
                            DateTime dt => dt.ToString("o", CultureInfo.InvariantCulture),
                            DateTimeOffset dto => dto.ToString("o", CultureInfo.InvariantCulture),
                            IFormattable formattable => formattable.ToString(null, CultureInfo.InvariantCulture),
                            _ => value?.ToString() ?? string.Empty
                        };

                        row[prop.Name].Set(formattedValue);
                    }
                }
            }, cancellationToken);
        }
    }
}
