using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;

namespace Proxirae.Presentation.WPF.Behaviors
{
    public class SelectAllBehavior
    {
        public static readonly DependencyProperty IsEnabledProperty =
            DependencyProperty.RegisterAttached(
                "IsEnabled",
                typeof(bool),
                typeof(SelectAllBehavior),
                new PropertyMetadata(false, OnIsEnabledChanged));

        public static void SetIsEnabled(DependencyObject element, bool value) => element.SetValue(IsEnabledProperty, value);
        public static bool GetIsEnabled(DependencyObject element) => (bool)element.GetValue(IsEnabledProperty);

        private static void OnIsEnabledChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            if (d is ListBox listBox)
            {
                if ((bool)e.NewValue)
                {
                    listBox.CommandBindings.Add(new CommandBinding(
                        ApplicationCommands.SelectAll,
                        (s, args) => ((ListBox)s).SelectAll(),
                        (s, args) => args.CanExecute = true));
                }
            }
        }
    }
}
