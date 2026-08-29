using System.Globalization;
using System.Windows.Data;

namespace Proxirae.Presentation.WPF.Converters
{
    public class SecondsToTimeConverters : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value is not ulong)
            {
                return "0.00:00:00";
            }

            double totalSeconds = System.Convert.ToDouble(value);

            TimeSpan timeSpan = TimeSpan.FromSeconds(totalSeconds);
            
            if (timeSpan.Days > 0)
            {
                return timeSpan.ToString(@"d\.hh\:mm\:ss");
            } else
            {
                return timeSpan.ToString(@"hh\:mm\:ss");
            }
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotSupportedException();
        }
    }
}
