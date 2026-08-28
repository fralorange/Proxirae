using System.Windows;
using System.Windows.Controls;

namespace Proxirae.Presentation.WPF.Behaviors
{
    public class AutoScrollBehavior
    {
        public static readonly DependencyProperty AutoScrollProperty =
            DependencyProperty.RegisterAttached("AutoScroll", typeof(bool), typeof(AutoScrollBehavior),
                new PropertyMetadata(false, AutoScrollPropertyChanged));

        public static void SetAutoScroll(DependencyObject obj, bool value) => obj.SetValue(AutoScrollProperty, value);
        public static bool GetAutoScroll(DependencyObject obj) => (bool)obj.GetValue(AutoScrollProperty);

        private static void AutoScrollPropertyChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            if (d is ScrollViewer scrollViewer && e.NewValue is bool autoScroll)
            {
                if (autoScroll)
                {
                    scrollViewer.ScrollChanged += OnScrollChanged;
                } else
                {
                    scrollViewer.ScrollChanged -= OnScrollChanged;
                }
            }
        }

        private static void OnScrollChanged(object sender, ScrollChangedEventArgs e)
        {
            if (e.ExtentHeightChange > 0 && sender is ScrollViewer s)
            {
                s.ScrollToBottom();
            }
        }
    }
}
