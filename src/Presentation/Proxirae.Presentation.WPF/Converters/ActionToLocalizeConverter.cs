using Proxirae.Contracts.DTOs.Rules.Actions;
using RentADeveloper.ResXLocalization;
using System.Globalization;
using System.Windows.Data;

namespace Proxirae.Presentation.WPF.Converters
{
    public class ActionToLocalizeConverter : IMultiValueConverter
    {
        public object Convert(object[] values, Type targetType, object parameter, CultureInfo culture)
        {
            if (values.Length == 0 || values[0] is not BaseActionDto action)
                return string.Empty;

            if (action.Name == "Direct")
                return Localizer.Current.Get("ActionNameDirect");

            if (action.Name == "Block")
                return Localizer.Current.Get("ActionNameBlock");

            if (action.Name.StartsWith("Proxy "))
            {
                string proxyWord = Localizer.Current.Get("ActionNameProxy");
                string restOfName = action.Name.Substring(6);

                return $"{proxyWord} {restOfName}";
            }

            return action.Name;
        }

        public object[] ConvertBack(object value, Type[] targetTypes, object parameter, CultureInfo culture)
        {
            throw new NotSupportedException();
        }
    }
}