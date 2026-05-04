using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;

namespace Proxirae.Presentation.WPF.Behaviors
{
    public class DataGridRowCommandBehavior
    {
        public static readonly DependencyProperty MouseDoubleClickCommandProperty =
            DependencyProperty.RegisterAttached(
                "MouseDoubleClickCommand",
                typeof(ICommand),
                typeof(DataGridRowCommandBehavior),
                new FrameworkPropertyMetadata(default(ICommand), new PropertyChangedCallback(OnSet))
        );

        public static readonly DependencyProperty MouseDoubleClickCommandParameterProperty =
            DependencyProperty.RegisterAttached(
                "MouseDoubleClickCommandParameter",
                typeof(object),
                typeof(DataGridRowCommandBehavior),
                new PropertyMetadata(null)
        );

        public static ICommand GetMouseDoubleClickCommand(DataGridRow target) =>
            (ICommand)target.GetValue(MouseDoubleClickCommandProperty);

        public static void SetMouseDoubleClickCommand(DataGridRow target, ICommand value) =>
            target.SetValue(MouseDoubleClickCommandProperty, value);

        public static object GetMouseDoubleClickCommandParameter(DataGridRow target) =>
            target.GetValue(MouseDoubleClickCommandParameterProperty);

        public static void SetMouseDoubleClickCommandParameter(DataGridRow target, object value) =>
            target.SetValue(MouseDoubleClickCommandParameterProperty, value);

        private static void OnSet(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            if (d is DataGridRow row)
            {
                row.MouseDoubleClick -= Row_MouseDoubleClick;
                row.MouseDoubleClick += Row_MouseDoubleClick;
            }
        }

        private static void Row_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            DataGridRow row = (DataGridRow)sender;

            var command = GetMouseDoubleClickCommand(row);
            var parameter = GetMouseDoubleClickCommandParameter(row);

            if (command?.CanExecute(parameter) == true)
                command.Execute(parameter);
        }
    }
}
