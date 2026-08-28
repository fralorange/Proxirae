using System.Globalization;
using System.Windows.Data;

namespace Proxirae.Presentation.WPF.Converters
{
    public class EnumToBooleanConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value == null || parameter == null)
            {
                return false;
            }

            return value.ToString()!.Equals(parameter.ToString());
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value != null && parameter != null && (bool)value)
            {
                return Enum.Parse(targetType, parameter.ToString()!);
            }

            return Binding.DoNothing;
        }
    }
}
