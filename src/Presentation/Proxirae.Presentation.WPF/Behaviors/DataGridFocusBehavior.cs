using Proxirae.Presentation.WPF.Controls;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;

namespace Proxirae.Presentation.WPF.Behaviors
{
    public static class DataGridFocusBehavior
    {
        public static readonly DependencyProperty ClearSelectionOnClickProperty =
            DependencyProperty.RegisterAttached(
                "ClearSelectionOnClick",
                typeof(bool),
                typeof(DataGridFocusBehavior),
                new PropertyMetadata(false, OnClearSelectionOnClickChanged));

        public static bool GetClearSelectionOnClick(DependencyObject obj)
            => (bool)obj.GetValue(ClearSelectionOnClickProperty);

        public static void SetClearSelectionOnClick(DependencyObject obj, bool value)
            => obj.SetValue(ClearSelectionOnClickProperty, value);

        private static void OnClearSelectionOnClickChanged(
            DependencyObject d,
            DependencyPropertyChangedEventArgs e)
        {
            if (d is not DataGrid dataGrid)
            {
                return;
            }

            if ((bool)e.NewValue)
            {
                dataGrid.PreviewMouseLeftButtonDown += DataGrid_PreviewMouseLeftButtonDown;
            }
            else
            {
                dataGrid.PreviewMouseLeftButtonDown -= DataGrid_PreviewMouseLeftButtonDown;
            }
        }

        private static void DataGrid_PreviewMouseLeftButtonDown(
            object sender,
            MouseButtonEventArgs e)
        {
            if (sender is not DataGrid grid ||
                e.OriginalSource is not DependencyObject source)
            {
                return;
            }

            if (IsInsideOpenMultiComboBoxDropdown(grid))
            {
                return;
            }

            var dep = source;

            while (dep is not null && dep is not DataGridRow)
            {
                dep = VisualTreeHelper.GetParent(dep);
            }

            if (dep is null)
            {
                grid.SelectedItem = null;
            }
        }

        private static bool IsInsideOpenMultiComboBoxDropdown(DataGrid grid)
        {
            foreach (var multiComboBox in FindVisualChildren<MultiComboBox>(grid))
            {
                if (multiComboBox.IsOpen &&
                    multiComboBox.IsDropDownMouseOver)
                {
                    return true;
                }
            }

            return false;
        }

        private static IEnumerable<T> FindVisualChildren<T>(DependencyObject root)
            where T : DependencyObject
        {
            if (root is null)
            {
                yield break;
            }

            for (var i = 0; i < VisualTreeHelper.GetChildrenCount(root); i++)
            {
                var child = VisualTreeHelper.GetChild(root, i);

                if (child is T result)
                {
                    yield return result;
                }

                foreach (var descendant in FindVisualChildren<T>(child))
                {
                    yield return descendant;
                }
            }
        }
    }
}