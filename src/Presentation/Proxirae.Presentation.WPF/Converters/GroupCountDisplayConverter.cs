using System.Globalization;
using System.Windows.Data;

namespace Proxirae.Presentation.WPF.Converters
{
    public sealed class GroupCountDisplayConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value is not int count)
                return string.Empty;

            return count > 99
                ? "99+"
                : count.ToString(culture);
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotSupportedException();
        }
    }
}
