using Proxirae.Application.Stores;
using System.Text.Json;

namespace Proxirae.Infrastructure.Persistence.JSON
{
    public class JsonPersistence<T> : IPersistence<T> where T : class
    {
        protected readonly List<T> _items = [];
        protected readonly string _filePath;
        protected bool _loaded;

        private readonly SemaphoreSlim _semaphore = new(1, 1);

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
            if (_loaded) return;
            await ReloadAsync(cancellationToken);
        }

        public async Task SaveAsync(CancellationToken cancellationToken)
        {
            await _semaphore.WaitAsync(cancellationToken);
            try
            {
                var json = JsonSerializer.Serialize(_items, JsonOptions);

                var tempFilePath = _filePath + ".tmp";

                await File.WriteAllTextAsync(tempFilePath, json, cancellationToken);

                File.Move(tempFilePath, _filePath, overwrite: true);
            } 
            finally
            {
                _semaphore.Release();
            }
        }

        public async Task ReloadAsync(CancellationToken cancellationToken)
        {
            await _semaphore.WaitAsync(cancellationToken);
            try
            {
                _items.Clear();
                _loaded = false;

                if (File.Exists(_filePath))
                {
                    var json = await File.ReadAllTextAsync(_filePath, cancellationToken);
                    var items = JsonSerializer.Deserialize<List<T>>(json, JsonOptions);

                    if (items is not null)
                    {
                        _items.AddRange(items);
                    }
                }

                _loaded = true;
            }
            finally
            {
                _semaphore.Release();
            }
        }
    }
}
