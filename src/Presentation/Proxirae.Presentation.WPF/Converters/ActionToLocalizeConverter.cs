using Proxirae.Contracts.DTOs.Rules.Actions;
using System.Globalization;
using System.Windows.Data;
using LocalizationKeys = Proxirae.Presentation.WPF.Resources.Localization.Localization;

namespace Proxirae.Presentation.WPF.Converters
{
    public class ActionToLocalizeConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value is BaseActionDto action)
            {
                if (action.Name == "Direct")
                    return LocalizationKeys.ActionNameDirect;

                if (action.Name == "Block")
                    return LocalizationKeys.ActionNameBlock;

                if (action.Name.StartsWith("Proxy "))
                {
                    string proxyWord = LocalizationKeys.ActionNameProxy;
                    string restOfName = action.Name.Substring(6);

                    return $"{proxyWord} {restOfName}";
                }

                return action.Name; 
            }

            return string.Empty;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
