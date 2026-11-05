using Proxirae.Presentation.WPF.Controls;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Media;

namespace Proxirae.Presentation.WPF.Behaviors
{
    public class ClearSelectionBehavior
    {
        public static readonly DependencyProperty IsEnabledProperty =
            DependencyProperty.RegisterAttached(
                "IsEnabled",
                typeof(bool),
                typeof(ClearSelectionBehavior),
                new PropertyMetadata(false, OnIsEnabledChanged));

        public static bool GetIsEnabled(DependencyObject obj)
            => (bool)obj.GetValue(IsEnabledProperty);

        public static void SetIsEnabled(DependencyObject obj, bool value)
            => obj.SetValue(IsEnabledProperty, value);

        private static void OnIsEnabledChanged(
            DependencyObject d,
            DependencyPropertyChangedEventArgs e)
        {
            if (d is not Control control)
                return;

            if ((bool)e.NewValue)
                control.PreviewMouseDown += Control_PreviewMouseDown;
            else
                control.PreviewMouseDown -= Control_PreviewMouseDown;
        }

        private static void Control_PreviewMouseDown(
            object sender,
            System.Windows.Input.MouseButtonEventArgs e)
        {
            if (sender is not Control control ||
                e.OriginalSource is not DependencyObject source)
            {
                return;
            }

            if (FindParent<ScrollBar>(source) is not null)
                return;

            bool isDataGridRowClicked =
                FindParent<DataGridRow>(source) is not null;

            bool isListBoxItemClicked =
                FindParent<ListBoxItem>(source) is not null;

            switch (control)
            {
                case TreeGrid treeGrid:
                    if (!isDataGridRowClicked)
                        treeGrid.InnerDataGrid.UnselectAll();
                    break;

                case DataGrid dataGrid:
                    if (!isDataGridRowClicked)
                        dataGrid.UnselectAll();
                    break;

                case ListBox listBox:
                    if (!isListBoxItemClicked)
                        listBox.UnselectAll();
                    break;
            }
        }

        private static T? FindParent<T>(DependencyObject child)
            where T : DependencyObject
        {
            DependencyObject parentObject =
                VisualTreeHelper.GetParent(child);

            if (parentObject == null)
                return null;

            if (parentObject is T parent)
                return parent;

            return FindParent<T>(parentObject);
        }
    }
}