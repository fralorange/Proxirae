using Proxirae.Application.Services.Styles;
using System.Windows;

namespace Proxirae.Presentation.WPF.Services.Styles
{
    public class WpfStylesService : IStylesService
    {
        public void ApplyGlobalTweaks()
        {
            EventManager.RegisterClassHandler(
                typeof(FrameworkElement),
                UIElement.GotFocusEvent,
                new RoutedEventHandler(RemoveFocusVisualStyle)
            );
        }

        private void RemoveFocusVisualStyle(object sender, RoutedEventArgs e)
        {
            if (sender is FrameworkElement element)
            {
                element.FocusVisualStyle = null;
            }
        }
    }
}
