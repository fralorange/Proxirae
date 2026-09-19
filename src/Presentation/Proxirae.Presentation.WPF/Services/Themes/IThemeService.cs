using Proxirae.Presentation.WPF.Models.Themes;

namespace Proxirae.Presentation.WPF.Services.Themes
{
    public interface IThemeService
    {
        Theme Current { get; }
        void Apply(Theme theme);
    }
}
