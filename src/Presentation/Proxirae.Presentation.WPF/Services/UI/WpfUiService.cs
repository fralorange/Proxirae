using Proxirae.Application.Services.UI;
using System.Windows;

namespace Proxirae.Presentation.WPF.Services.UI
{
    public class WpfUiService : IUiService
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
