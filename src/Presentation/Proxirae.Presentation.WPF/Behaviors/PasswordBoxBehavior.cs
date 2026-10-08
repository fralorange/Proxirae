using System.Windows;
using System.Windows.Controls;

namespace Proxirae.Presentation.WPF.Behaviors
{
    public static class PasswordBoxBehavior
    {
        public static readonly DependencyProperty PasswordProperty =
            DependencyProperty.RegisterAttached(
                "Password",
                typeof(string),
                typeof(PasswordBoxBehavior),
                new FrameworkPropertyMetadata(
                    null,
                    FrameworkPropertyMetadataOptions.BindsTwoWayByDefault,
                    OnPasswordChanged));

        public static readonly DependencyProperty IsAttachedProperty =
            DependencyProperty.RegisterAttached(
                "IsAttached",
                typeof(bool),
                typeof(PasswordBoxBehavior),
                new PropertyMetadata(false, OnIsAttachedChanged));

        public static void SetPassword(DependencyObject element, string? value)
        {
            element.SetValue(PasswordProperty, value);
        }

        public static string? GetPassword(DependencyObject element)
        {
            return (string?)element.GetValue(PasswordProperty);
        }

        public static void SetIsAttached(DependencyObject element, bool value)
        {
            element.SetValue(IsAttachedProperty, value);
        }

        public static bool GetIsAttached(DependencyObject element)
        {
            return (bool)element.GetValue(IsAttachedProperty);
        }

        private static void OnIsAttachedChanged(
            DependencyObject d,
            DependencyPropertyChangedEventArgs e)
        {
            if (d is not PasswordBox passwordBox)
            {
                return;
            }

            if ((bool)e.NewValue)
            {
                passwordBox.PasswordChanged += OnPasswordBoxPasswordChanged;
            }
            else
            {
                passwordBox.PasswordChanged -= OnPasswordBoxPasswordChanged;
            }
        }

        private static void OnPasswordChanged(
            DependencyObject d,
            DependencyPropertyChangedEventArgs e)
        {
            if (d is not PasswordBox passwordBox)
            {
                return;
            }

            var password = (string?)e.NewValue ?? string.Empty;

            if (passwordBox.Password != password)
            {
                passwordBox.Password = password;
            }
        }

        private static void OnPasswordBoxPasswordChanged(
            object sender,
            RoutedEventArgs e)
        {
            var passwordBox = (PasswordBox)sender;

            passwordBox.SetCurrentValue(
                PasswordProperty,
                passwordBox.Password);
        }
    }
}