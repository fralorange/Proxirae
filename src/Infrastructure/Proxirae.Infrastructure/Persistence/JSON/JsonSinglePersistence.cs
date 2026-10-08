using Proxirae.Application.Persistence;
using System.Text.Json;

namespace Proxirae.Infrastructure.Persistence.JSON
{
    public class JsonSinglePersistence<T> : IPersistence<T>
        where T : class, new()
    {
        protected T _item = new();
        protected readonly string _filePath;
        protected bool _loaded;

        private readonly SemaphoreSlim _semaphore = new(1, 1);

        private static readonly JsonSerializerOptions JsonOptions = new()
        {
            WriteIndented = true
        };

        public JsonSinglePersistence(string filePath)
        {
            _filePath = filePath;
        }

        protected async Task EnsureLoadedAsync(
            CancellationToken cancellationToken)
        {
            await _semaphore.WaitAsync(cancellationToken);

            try
            {
                if (_loaded)
                {
                    return;
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
            T item = new();
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
                        var deserialized =
                            JsonSerializer.Deserialize<T>(
                                json,
                                JsonOptions);

                        if (deserialized is null)
                        {
                            rewriteRequired = true;
                        }
                        else
                        {
                            item = deserialized;
                        }
                    }
                }
                catch (JsonException)
                {
                    item = new();
                    rewriteRequired = true;
                }
                catch (NotSupportedException)
                {
                    item = new();
                    rewriteRequired = true;
                }
            }

            _item = item;
            _loaded = true;

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
            var dataToSave = _item ?? new();

            var json = JsonSerializer.Serialize(
                dataToSave,
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