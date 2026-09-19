using System.Windows;
using System.Windows.Input;

namespace Proxirae.Presentation.WPF.Behaviors
{
    public static class NumericInputBehavior
    {
        public static readonly DependencyProperty IsIntegerOnlyProperty =
            DependencyProperty.RegisterAttached(
                "IsIntegerOnly",
                typeof(bool),
                typeof(NumericInputBehavior),
                new PropertyMetadata(false, OnIsIntegerOnlyChanged));

        public static void SetIsIntegerOnly(DependencyObject element, bool value)
            => element.SetValue(IsIntegerOnlyProperty, value);

        public static bool GetIsIntegerOnly(DependencyObject element)
            => (bool)element.GetValue(IsIntegerOnlyProperty);

        private static void OnIsIntegerOnlyChanged(
            DependencyObject d,
            DependencyPropertyChangedEventArgs e)
        {
            if (d is not UIElement element)
                return;

            if ((bool)e.NewValue)
            {
                element.PreviewTextInput += OnPreviewTextInput;
                DataObject.AddPastingHandler(element, OnPaste);
            }
            else
            {
                element.PreviewTextInput -= OnPreviewTextInput;
                DataObject.RemovePastingHandler(element, OnPaste);
            }
        }

        private static void OnPreviewTextInput(
            object sender,
            TextCompositionEventArgs e)
        {
            e.Handled = e.Text.Any(c => !char.IsDigit(c));
        }

        private static void OnPaste(
            object sender,
            DataObjectPastingEventArgs e)
        {
            if (!e.DataObject.GetDataPresent(DataFormats.Text))
            {
                e.CancelCommand();
                return;
            }

            var text = e.DataObject.GetData(DataFormats.Text) as string;

            if (string.IsNullOrEmpty(text) ||
                text.Any(c => !char.IsDigit(c)))
            {
                e.CancelCommand();
            }
        }
    }
}
