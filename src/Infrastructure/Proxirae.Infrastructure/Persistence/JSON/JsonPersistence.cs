using Proxirae.Application.Persistence;
using System.Text.Json;

namespace Proxirae.Infrastructure.Persistence.JSON
{
    public class JsonPersistence<T> : IPersistence<T>
        where T : class
    {
        protected readonly List<T> _items = [];
        protected readonly string _filePath;
        protected bool _loaded;

        private readonly SemaphoreSlim _semaphore = new(1, 1);

        private static readonly JsonSerializerOptions JsonOptions = new()
        {
            WriteIndented = true,
        };

        public JsonPersistence(string filePath)
        {
            _filePath = filePath;
        }

        protected async Task EnsureLoadedAsync(
            CancellationToken cancellationToken)
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

                await UnsafeReloadAsync(cancellationToken);
            }
            finally
            {
                _semaphore.Release();
            }
        }

        public async Task SaveAsync(
            CancellationToken cancellationToken)
        {
            await EnsureLoadedAsync(cancellationToken);

            await _semaphore.WaitAsync(cancellationToken);

            try
            {
                await SaveUnsafeAsync(cancellationToken);
            }
            finally
            {
                _semaphore.Release();
            }
        }

        public async Task ReloadAsync(
            CancellationToken cancellationToken)
        {
            await _semaphore.WaitAsync(cancellationToken);

            try
            {
                await UnsafeReloadAsync(cancellationToken);
            }
            finally
            {
                _semaphore.Release();
            }
        }

        private async Task UnsafeReloadAsync(
            CancellationToken cancellationToken)
        {
            List<T> items = [];
            bool rewriteRequired = false;

            if (!File.Exists(_filePath))
            {
                rewriteRequired = true;
            }
            else
            {
                try
                {
                    var json = await File.ReadAllTextAsync(
                        _filePath,
                        cancellationToken);

                    if (string.IsNullOrWhiteSpace(json))
                    {
                        rewriteRequired = true;
                    }
                    else
                    {
                        var deserialized = JsonSerializer.Deserialize<List<T>>(
                            json,
                            JsonOptions);

                        if (deserialized is null)
                        {
                            rewriteRequired = true;
                        }
                        else
                        {
                            items = deserialized;

                            if (items.Any(item => item is null))
                            {
                                items = items
                                    .Where(item => item is not null)
                                    .ToList();

                                rewriteRequired = true;
                            }
                        }
                    }
                }
                catch (JsonException)
                {
                    items = [];
                    rewriteRequired = true;
                }
                catch (NotSupportedException)
                {
                    items = [];
                    rewriteRequired = true;
                }
            }

            lock (_items)
            {
                _items.Clear();
                _items.AddRange(items);
                _loaded = true;
            }

            if (rewriteRequired)
            {
                try
                {
                    await SaveUnsafeAsync(cancellationToken);
                }
                catch (IOException)
                {
                }
                catch (UnauthorizedAccessException)
                {
                }
            }
        }

        private async Task SaveUnsafeAsync(
            CancellationToken cancellationToken)
        {
            List<T> snapshot;

            lock (_items)
            {
                snapshot = _items.ToList();
            }

            var json = JsonSerializer.Serialize(
                snapshot,
                JsonOptions);

            var directory = Path.GetDirectoryName(_filePath);

            if (!string.IsNullOrEmpty(directory))
            {
                Directory.CreateDirectory(directory);
            }

            var tempFilePath = _filePath + ".tmp";

            try
            {
                await File.WriteAllTextAsync(
                    tempFilePath,
                    json,
                    cancellationToken);

                File.Move(
                    tempFilePath,
                    _filePath,
                    overwrite: true);
            }
            finally
            {
                if (File.Exists(tempFilePath))
                {
                    try
                    {
                        File.Delete(tempFilePath);
                    }
                    catch
                    {
                    }
                }
            }
        }
    }
}