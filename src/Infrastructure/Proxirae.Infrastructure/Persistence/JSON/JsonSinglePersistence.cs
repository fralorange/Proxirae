using Proxirae.Application.Stores;
using System.Text.Json;

namespace Proxirae.Infrastructure.Persistence.JSON
{
    public class JsonSinglePersistence<T> : IPersistence<T> where T : class, new()
    {
        protected T _item = default!;
        protected readonly string _filePath;
        protected bool _loaded;

        private static readonly JsonSerializerOptions JsonOptions = new()
        {
            WriteIndented = true
        };

        public JsonSinglePersistence(string filePath)
        {
            _filePath = filePath;
        }

        protected async Task EnsureLoadedAsync(CancellationToken cancellationToken)
        {
            if (_loaded)
                return;

            if (!File.Exists(_filePath))
            {
                _item = new T();
                _loaded = true;
                return;
            }

            var json = await File.ReadAllTextAsync(_filePath, cancellationToken);

            _item = JsonSerializer.Deserialize<T>(json, JsonOptions) ?? new T();
            _loaded = true;
        }

        public async Task SaveAsync(CancellationToken cancellationToken)
        {
            var dataToSave = _item ?? new T();

            var json = JsonSerializer.Serialize(dataToSave, JsonOptions);

            var tempFilePath = _filePath + ".tmp";

            await File.WriteAllTextAsync(tempFilePath, json, cancellationToken);

            File.Move(tempFilePath, _filePath, overwrite: true);
        }
    }
}
