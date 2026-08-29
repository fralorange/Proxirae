using Proxirae.Application.Stores;
using System.Text.Json;

namespace Proxirae.Infrastructure.Persistence.JSON
{
    public class JsonPersistence<T> : IPersistence<T> where T : class
    {
        protected readonly List<T> _items = [];
        protected readonly string _filePath;
        protected bool _loaded;

        public JsonPersistence(string filePath)
        {
            _filePath = filePath;
        }

        private static readonly JsonSerializerOptions JsonOptions = new()
        {
            WriteIndented = true,
        };

        protected async Task EnsureLoadedAsync(CancellationToken cancellationToken)
        {
            if (_loaded)
                return;

            if (!File.Exists(_filePath))
            {
                _loaded = true;
                return;
            }

            var json = await File.ReadAllTextAsync(_filePath, cancellationToken);

            var items = JsonSerializer.Deserialize<List<T>>(json, JsonOptions);

            if (items is not null)
            {
                _items.AddRange(items);
            }

            _loaded = true;
        }

        public async Task SaveAsync(CancellationToken cancellationToken)
        {
            var json = JsonSerializer.Serialize(_items, JsonOptions);

            var tempFilePath = _filePath + ".tmp";

            await File.WriteAllTextAsync(tempFilePath, json, cancellationToken);

            File.Move(tempFilePath, _filePath, overwrite: true);
        }
    }
}
