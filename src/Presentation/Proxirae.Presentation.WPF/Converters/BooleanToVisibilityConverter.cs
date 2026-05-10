using System.Globalization;
using System.Windows;
using System.Windows.Data;

namespace Proxirae.Presentation.WPF.Converters
{
    public class BooleanToVisibilityConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            bool flag = value is bool b && b;

            bool useHidden = parameter?.ToString() == "Hidden";

            if (flag) return Visibility.Visible;

            return useHidden ? Visibility.Hidden : Visibility.Collapsed;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
