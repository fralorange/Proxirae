using Proxirae.Application.Security.Cryptography;
using Proxirae.Application.Security.Protection;
using Proxirae.Application.Services.Archive;
using Proxirae.Application.Services.Configuration;
using Proxirae.Contracts.DTOs.Proxies;
using System.Text.Json;

namespace Proxirae.Application.Services.Package
{
    public class PackageService : IPackageService
    {
        private readonly IArchiveService _archiveService;
        private readonly IConfigurationService _configurationService;
        private readonly IProtector _protector;
        private readonly ICryptographer _cryptographer;
        private readonly JsonSerializerOptions _options = new() { WriteIndented = true };

        public PackageService(
            IArchiveService archiveService,
            IConfigurationService configurationService,
            IProtector protector,
            ICryptographer cryptographer)
        {
            _archiveService = archiveService;
            _configurationService = configurationService;
            _protector = protector;
            _cryptographer = cryptographer;
        }

        public async Task<bool?> ExportAsync(
            string archivePath,
            Func<CancellationToken, Task<string?>> masterPasswordProvider,
            CancellationToken token = default)
        {
            var archiveEntries = new Dictionary<string, byte[]>(StringComparer.OrdinalIgnoreCase);

            string rulesPath = _configurationService.RulesFilePath;
            string proxiesPath = _configurationService.ProxiesFilePath;

            if (File.Exists(rulesPath))
            {
                archiveEntries[Path.GetFileName(rulesPath)] = await File.ReadAllBytesAsync(rulesPath, token);
            }
            else
            {
                return false;
            }

            if (File.Exists(proxiesPath))
            {
                using var fs = File.OpenRead(proxiesPath);
                var proxies = await JsonSerializer.DeserializeAsync<List<ProxyDetailDto>>(fs, cancellationToken: token);

                if (proxies != null)
                {
                    string? masterPassword = null;
                    bool masterPasswordRequested = false;

                    foreach (var proxy in proxies)
                    {
                        if (string.IsNullOrEmpty(proxy.Password))
                        {
                            continue;
                        }

                        string plainText;
                        if (_protector.IsProtected(proxy.Password))
                        {
                            if (!_protector.TryUnprotect(proxy.Password, out plainText))
                            {
                                return false;
                            }
                        }
                        else
                        {
                            plainText = proxy.Password;
                        }

                        if (string.IsNullOrEmpty(plainText))
                        {
                            continue;
                        }

                        if (!masterPasswordRequested)
                        {
                            masterPassword = await masterPasswordProvider(token);
                            masterPasswordRequested = true;

                            if (string.IsNullOrWhiteSpace(masterPassword))
                            {
                                return null; 
                            }
                        }

                        proxy.Password = _cryptographer.Encrypt(plainText, masterPassword!);
                    }

                    using var ms = new MemoryStream();
                    await JsonSerializer.SerializeAsync(ms, proxies, _options, token);
                    archiveEntries[Path.GetFileName(proxiesPath)] = ms.ToArray();
                }
            }
            else
            {
                return false;
            }

            return _archiveService.CreateArchive(archivePath, archiveEntries);
        }

        public async Task<bool> ImportAsync(
            string archivePath,
            Func<CancellationToken, Task<string?>> masterPasswordProvider,
            CancellationToken token = default)
        {
            var archiveEntries = _archiveService.ExtractArchive(archivePath);
            if (archiveEntries == null || archiveEntries.Count == 0)
            {
                return false;
            }

            var rulesPath = _configurationService.RulesFilePath;
            var proxiesPath = _configurationService.ProxiesFilePath;

            var rulesFileName = Path.GetFileName(rulesPath);
            if (archiveEntries.TryGetValue(rulesFileName, out var rulesBytes))
            {
                await File.WriteAllBytesAsync(rulesPath, rulesBytes, token);
            }

            var proxiesFileName = Path.GetFileName(proxiesPath);
            if (archiveEntries.TryGetValue(proxiesFileName, out var proxyBytes))
            {
                using var ms = new MemoryStream(proxyBytes);
                var proxies = await JsonSerializer.DeserializeAsync<List<ProxyDetailDto>>(ms, cancellationToken: token);

                if (proxies != null)
                {
                    string? masterPassword = null;
                    bool masterPasswordRequested = false;

                    foreach (var proxy in proxies)
                    {
                        if (string.IsNullOrEmpty(proxy.Password))
                        {
                            continue;
                        }

                        if (_cryptographer.IsEncrypted(proxy.Password))
                        {
                            if (!masterPasswordRequested)
                            {
                                masterPassword = await masterPasswordProvider(token);
                                masterPasswordRequested = true;
                            }

                            if (string.IsNullOrWhiteSpace(masterPassword))
                            {
                                proxy.Password = string.Empty;
                            }
                            else if (_cryptographer.TryDecrypt(proxy.Password, masterPassword, out var plainText))
                            {
                                proxy.Password = _protector.Protect(plainText);
                            }
                            else
                            {
                                return false;
                            }
                        }
                        else
                        {
                            if (!_protector.IsProtected(proxy.Password))
                            {
                                proxy.Password = _protector.Protect(proxy.Password);
                            }
                        }
                    }

                    using var fs = new FileStream(proxiesPath, FileMode.Create, FileAccess.Write);
                    await JsonSerializer.SerializeAsync(fs, proxies, _options, token);
                }
            }

            await _configurationService.ReloadAsync(token);
            return true;
        }
    }
}