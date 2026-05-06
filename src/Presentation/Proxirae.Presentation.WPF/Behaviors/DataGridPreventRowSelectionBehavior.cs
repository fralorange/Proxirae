using Microsoft.Xaml.Behaviors;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;

namespace Proxirae.Presentation.WPF.Behaviors
{
    public class DataGridPreventRowSelectionBehavior : Behavior<DataGrid>
    {
        static int x = 0;
        protected override void OnAttached()
        {
            AssociatedObject.PreviewMouseLeftButtonDown += OnPreviewMouseDown;
        }

        protected override void OnDetaching()
        {
            AssociatedObject.PreviewMouseLeftButtonDown -= OnPreviewMouseDown;
        }

        private void OnPreviewMouseDown(object sender, MouseButtonEventArgs e)
        {
            if (e.OriginalSource is not DependencyObject dep)
                return;

            var checkBox = FindParent<CheckBox>(dep);
            var comboBox = FindParent<ComboBox>(dep);

            if (checkBox is not null || comboBox is not null )
            {
                e.Handled = true;
            }

            if (checkBox is not null)
            {
                checkBox.IsChecked = !checkBox.IsChecked;
            }

            if (comboBox is not null)
            {
                comboBox.Focus();
                comboBox.IsDropDownOpen = !comboBox.IsDropDownOpen;
            }
        }

        private static T? FindParent<T>(DependencyObject? child) where T : DependencyObject
        {
            while (child != null)
            {
                if (child is T t)
                    return t;

                child = VisualTreeHelper.GetParent(child);
            }
            return null;
        }
    }
}
