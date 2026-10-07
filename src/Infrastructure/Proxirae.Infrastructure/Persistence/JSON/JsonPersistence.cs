using Proxirae.Application.Persistence;
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
            await _semaphore.WaitAsync(cancellationToken);
            try
            {
                lock (_items)
                {
                    if (_loaded)
                    {
                        return;
                    }
                }

                await ReloadCoreAsync(cancellationToken);
            }
            finally
            {
                _semaphore.Release();
            }
        }

        public async Task SaveAsync(CancellationToken cancellationToken)
        {
            await EnsureLoadedAsync(cancellationToken);

            await _semaphore.WaitAsync(cancellationToken);
            try
            {
                List<T> snapshot;
                lock (_items)
                {
                    snapshot = _items.ToList();
                }

                var json = JsonSerializer.Serialize(snapshot, JsonOptions);
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
                await ReloadCoreAsync(cancellationToken);
            }
            finally
            {
                _semaphore.Release();
            }
        }

        private async Task ReloadCoreAsync(CancellationToken cancellationToken)
        {
            List<T>? items = null;

            if (File.Exists(_filePath))
            {
                var json = await File.ReadAllTextAsync(_filePath, cancellationToken);
                items = JsonSerializer.Deserialize<List<T>>(json, JsonOptions);
            }

            lock (_items)
            {
                _items.Clear();
                if (items is not null)
                {
                    _items.AddRange(items);
                }
                _loaded = true;
            }
        }
    }
}
