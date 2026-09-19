using Microsoft.Extensions.Caching.Memory;
using Proxirae.Presentation.WPF.Models.Process;
using System.IO;
using System.Management;
using System.Windows;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Media.Imaging;

namespace Proxirae.Presentation.WPF.Services.Process
{
    public class ProcessInfoService : IProcessInfoService
    {
        private readonly IMemoryCache _cache;

        public ProcessInfoService(IMemoryCache cache)
        {
            _cache = cache;
        }

        public async Task<ProcessInfo?> GetProcessInfoAsync(long processId, CancellationToken cancellationToken)
        {
            if (processId < int.MinValue && processId > int.MaxValue) return null;

            return await _cache.GetOrCreateAsync(
                key: $"ProcessInfo_{processId}",
                factory: async entry =>
                {
                    entry.SetSize(1);
                    entry.SetSlidingExpiration(TimeSpan.FromMinutes(10));

                    return await Task.Run(() => FetchProcessInfo((int)processId), cancellationToken);
                }
            );
        }

        private ProcessInfo FetchProcessInfo(int processId)
        {
            string name = "Unknown";
            ImageSource? iconSource = null;
            string? filePath = null;

            try
            {
                string query = $"SELECT ExecutablePath FROM Win32_Process WHERE ProcessId = {processId}";
                using var searcher = new ManagementObjectSearcher(query);
                using var result = searcher.Get();

                foreach (var mo in result)
                {
                    filePath = mo["ExecutablePath"]?.ToString();
                    break;
                }

                if (!string.IsNullOrEmpty(filePath))
                {
                    name = Path.GetFileName(filePath);

                    using var icon = System.Drawing.Icon.ExtractAssociatedIcon(filePath);
                    if (icon is not null)
                    {
                        iconSource = Imaging.CreateBitmapSourceFromHIcon(
                            icon.Handle,
                            Int32Rect.Empty,
                            BitmapSizeOptions.FromEmptyOptions());

                        iconSource.Freeze();
                    }
                } else
                {
                    using var process = System.Diagnostics.Process.GetProcessById(processId);
                    name = process.ProcessName + ".exe";
                }
            } catch { }

            return new(name, iconSource);
        }
    }
}
