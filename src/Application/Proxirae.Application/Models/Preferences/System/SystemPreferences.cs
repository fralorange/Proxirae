namespace Proxirae.Application.Models.Preferences.System
{
    public record SystemPreferences
    {
        public bool IsAutostartEnabled { get; init; } = false;
        public string LanguageCode { get; init; } = "en-US";
    }
}
