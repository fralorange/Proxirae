using Proxirae.Application.Services.Localization;
using RentADeveloper.ResXLocalization;
using System.Globalization;

namespace Proxirae.Presentation.WPF.Services.Localization
{
    public class LocalizationService : ILocalizationService
    {
        private bool _isInitialized;

        public void Initialize(string code)
        {
            if (_isInitialized) return;

            Localizer.Current.RegisterResourceManager(
            Resources.Localization.Localization.ResourceManager);

            _isInitialized = true;

            SwitchTo(code);
        }

        public void SwitchTo(string code)
        {
            if (!_isInitialized) return;

            try
            {
                Localizer.Current.CurrentCulture = new CultureInfo(code);
            } catch (CultureNotFoundException) { }
        }
    }
}
