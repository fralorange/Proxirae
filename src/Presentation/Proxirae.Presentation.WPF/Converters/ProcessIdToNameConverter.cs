using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Management;
using System.Windows.Data;

namespace Proxirae.Presentation.WPF.Converters
{
    // TODO: Cache Process data
    public class ProcessIdToNameConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            var name = "Unknown";

            int? nullableId = value switch
            {
                byte b => b,
                int i => i,
                long l when l >= int.MinValue && l <= int.MaxValue => (int)l,
                _ => null,
            };

            if (nullableId is int processId)
            {
                try
                {
                    string? filePath = null;
                    string query = $"SELECT ExecutablePath FROM Win32_Process WHERE ProcessId = {processId}";

                    using (var searcher = new ManagementObjectSearcher(query))
                    using (var results = searcher.Get())
                    {
                        foreach (ManagementBaseObject mo in results)
                        {
                            filePath = mo["ExecutablePath"]?.ToString();
                            break;
                        }
                    }

                    if (!string.IsNullOrEmpty(filePath))
                    {
                        name = Path.GetFileName(filePath);
                    }
                    else
                    {
                        using Process process = Process.GetProcessById(processId);
                        name = process.ProcessName + ".exe";
                    }
                }
                catch { }
            }

            return name;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}