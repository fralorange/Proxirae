using Proxirae.Application.Services.Configurator;
using System.Windows;

namespace Proxirae.Presentation.WPF.Services.Configurator
{
    public class WpfUiConfiguratorService : IConfiguratorService
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
