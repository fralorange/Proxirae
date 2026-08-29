using System.Diagnostics;
using System.Globalization;
using System.Windows.Data;
using System.Drawing;
using System.Windows;
using System.Windows.Interop;
using System.Windows.Media.Imaging;

namespace Proxirae.Presentation.WPF.Converters
{
    public class ProcessIdToIconConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
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

                    if (string.IsNullOrEmpty(filePath)) return DependencyProperty.UnsetValue;

                    using Icon? icon = Icon.ExtractAssociatedIcon(filePath);
                    if (icon is not null)
                    {
                        var source = Imaging.CreateBitmapSourceFromHIcon(
                            icon.Handle,
                            Int32Rect.Empty,
                            BitmapSizeOptions.FromEmptyOptions());

                        source.Freeze();
                        return source;
                    }
                } 
                catch
                {
                    return DependencyProperty.UnsetValue;
                }
            }

            return DependencyProperty.UnsetValue;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotSupportedException();
        }
    }
}
