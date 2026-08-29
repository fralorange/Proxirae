using Proxirae.Application.Models.Preferences.Appearance;
using Proxirae.Application.Models.Preferences.Engine;
using Proxirae.Application.Models.Preferences.System;

namespace Proxirae.Application.Models.Preferences
{
    public record Preferences
    {
        public SystemPreferences System { get; init; } = new();
        public EnginePreferences Engine { get; init; } = new();
        public AppearancePreferences Appearance { get; init; } = new();
    }
}
