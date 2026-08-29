namespace Proxirae.Application.Models.Preferences.Appearance
{
    public record AppearancePreferences
    {
        public string Theme { get; init; } = "System";
        public double TabsHeight { get; init; }
    }
}
