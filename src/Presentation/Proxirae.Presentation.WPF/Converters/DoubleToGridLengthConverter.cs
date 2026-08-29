using System.Globalization;
using System.Windows;
using System.Windows.Data;

namespace Proxirae.Presentation.WPF.Converters
{
    public class DoubleToGridLengthConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value is double d)
            {
                return new GridLength(d, GridUnitType.Pixel);
            }

            return new GridLength();
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value is GridLength gl)
            {
                return gl.Value;
            }

            return 0;
        }
    }
}
