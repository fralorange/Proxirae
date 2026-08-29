using System.Globalization;
using System.Windows.Data;

namespace Proxirae.Presentation.WPF.Converters
{
    public class BytesToSizeConverter : IValueConverter
    {
        private static readonly string[] Suffixes = ["B", "KB", "MB", "GB", "TB", "PB"];

        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value is not ulong)
            {
                return "0 B";
            }

            var bytes = System.Convert.ToDouble(value);

            int order = 0;
            while (bytes >= 1024 && order < Suffixes.Length -1)
            {
                order++;
                bytes /= 1024;
            }

            var format = order == 0 ? "0" : "0.##";

            return $"{bytes.ToString(format, CultureInfo.InvariantCulture)} {Suffixes[order]}";
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotSupportedException();
        }
    }
}
