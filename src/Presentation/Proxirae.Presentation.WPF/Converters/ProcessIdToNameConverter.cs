using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Windows.Data;

namespace Proxirae.Presentation.WPF.Converters
{
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
                    Process process = Process.GetProcessById(processId);
                    string? filePath = process.MainModule?.FileName;

                    if (!string.IsNullOrEmpty(filePath))
                    {
                        name = Path.GetFileName(filePath);
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
