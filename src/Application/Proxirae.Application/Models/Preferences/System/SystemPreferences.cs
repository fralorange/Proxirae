namespace Proxirae.Application.Models.Preferences.System
{
    public record SystemPreferences
    {
        public bool Autostart { get; init; } = false;
        public bool SilentStart { get; init;} = false;
        public bool StartMinimized { get; init; } = false;
        public string LanguageCode { get; init; } = "en-US";
    }
}
