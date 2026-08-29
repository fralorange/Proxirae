using System.Windows;
using System.Windows.Controls;
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

        private static void OnClearSelectionOnClickChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            if (d is DataGrid dataGrid)
            {
                if ((bool)e.NewValue)
                {
                    dataGrid.PreviewMouseLeftButtonDown += DataGrid_PreviewMouseLeftButtonDown;
                }
                else
                {
                    dataGrid.PreviewMouseLeftButtonDown -= DataGrid_PreviewMouseLeftButtonDown;
                }
            }
        }

        private static void DataGrid_PreviewMouseLeftButtonDown(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            var dep = (DependencyObject)e.OriginalSource;

            while (dep != null && dep is not DataGridRow)
            {
                dep = VisualTreeHelper.GetParent(dep);
            }

            if (dep == null && sender is DataGrid grid)
            {
                grid.SelectedItem = null;
            }
        }
    }
}
