namespace Proxirae.Application.Services.Localization
{
    public interface ILocalizationService
    {
        void Initialize(string code);
        void SwitchTo(string code);
    }
}
