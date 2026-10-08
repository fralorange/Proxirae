using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;

namespace Proxirae.Presentation.WPF.Behaviors;

public static class WatermarkBehavior
{
    public static readonly DependencyProperty WatermarkProperty =
        DependencyProperty.RegisterAttached(
            "Watermark",
            typeof(string),
            typeof(WatermarkBehavior),
            new PropertyMetadata(null, OnWatermarkChanged));

    public static readonly DependencyProperty WatermarkOpacityProperty =
        DependencyProperty.RegisterAttached(
            "WatermarkOpacity",
            typeof(double),
            typeof(WatermarkBehavior),
            new PropertyMetadata(0.45, OnWatermarkChanged));

    private static readonly DependencyProperty IsUpdatingProperty =
        DependencyProperty.RegisterAttached(
            "IsUpdating",
            typeof(bool),
            typeof(WatermarkBehavior),
            new PropertyMetadata(false));

    private static readonly DependencyProperty OriginalOpacityProperty =
        DependencyProperty.RegisterAttached(
            "OriginalOpacity",
            typeof(double),
            typeof(WatermarkBehavior),
            new PropertyMetadata(1.0));

    private static readonly DependencyProperty HasOriginalOpacityProperty =
        DependencyProperty.RegisterAttached(
            "HasOriginalOpacity",
            typeof(bool),
            typeof(WatermarkBehavior),
            new PropertyMetadata(false));

    public static void SetWatermark(DependencyObject element, string? value) =>
        element.SetValue(WatermarkProperty, value);

    public static string? GetWatermark(DependencyObject element) =>
        (string?)element.GetValue(WatermarkProperty);

    public static void SetWatermarkOpacity(DependencyObject element, double value) =>
        element.SetValue(WatermarkOpacityProperty, value);

    public static double GetWatermarkOpacity(DependencyObject element) =>
        (double)element.GetValue(WatermarkOpacityProperty);

    private static void SetIsUpdating(DependencyObject element, bool value) =>
        element.SetValue(IsUpdatingProperty, value);

    private static bool GetIsUpdating(DependencyObject element) =>
        (bool)element.GetValue(IsUpdatingProperty);

    private static void SetOriginalOpacity(DependencyObject element, double value) =>
        element.SetValue(OriginalOpacityProperty, value);

    private static double GetOriginalOpacity(DependencyObject element) =>
        (double)element.GetValue(OriginalOpacityProperty);

    private static void SetHasOriginalOpacity(DependencyObject element, bool value) =>
        element.SetValue(HasOriginalOpacityProperty, value);

    private static bool GetHasOriginalOpacity(DependencyObject element) =>
        (bool)element.GetValue(HasOriginalOpacityProperty);

    private static void OnWatermarkChanged(
        DependencyObject dependencyObject,
        DependencyPropertyChangedEventArgs e)
    {
        if (dependencyObject is not TextBlock textBlock)
            return;

        Detach(textBlock);

        if (!string.IsNullOrEmpty(GetWatermark(textBlock)))
            Attach(textBlock);
    }

    private static void Attach(TextBlock textBlock)
    {
        if (!GetHasOriginalOpacity(textBlock))
        {
            SetOriginalOpacity(textBlock, textBlock.Opacity);
            SetHasOriginalOpacity(textBlock, true);
        }

        textBlock.Loaded += OnLoaded;
        textBlock.Unloaded += OnUnloaded;

        DependencyPropertyDescriptor
            .FromProperty(TextBlock.TextProperty, typeof(TextBlock))
            ?.AddValueChanged(textBlock, OnTextChanged);

        Update(textBlock);
    }

    private static void Detach(TextBlock textBlock)
    {
        textBlock.Loaded -= OnLoaded;
        textBlock.Unloaded -= OnUnloaded;

        DependencyPropertyDescriptor
            .FromProperty(TextBlock.TextProperty, typeof(TextBlock))
            ?.RemoveValueChanged(textBlock, OnTextChanged);

        RestoreOpacity(textBlock);
    }

    private static void OnLoaded(object sender, RoutedEventArgs e)
    {
        if (sender is TextBlock textBlock)
            Update(textBlock);
    }

    private static void OnUnloaded(object sender, RoutedEventArgs e)
    {
        if (sender is TextBlock textBlock)
            RestoreOpacity(textBlock);
    }

    private static void OnTextChanged(object? sender, EventArgs e)
    {
        if (sender is not TextBlock textBlock)
            return;

        if (GetIsUpdating(textBlock))
            return;

        Update(textBlock);
    }

    private static void Update(TextBlock textBlock)
    {
        var watermark = GetWatermark(textBlock);

        if (string.IsNullOrEmpty(watermark))
        {
            RestoreOpacity(textBlock);
            return;
        }

        if (GetIsUpdating(textBlock))
            return;

        if (string.IsNullOrWhiteSpace(textBlock.Text))
        {
            SetIsUpdating(textBlock, true);

            try
            {
                textBlock.SetCurrentValue(
                    TextBlock.TextProperty,
                    watermark);

                textBlock.SetCurrentValue(
                    UIElement.OpacityProperty,
                    GetWatermarkOpacity(textBlock));
            }
            finally
            {
                SetIsUpdating(textBlock, false);
            }

            return;
        }

        if (textBlock.Text != watermark)
            RestoreOpacity(textBlock);
    }

    private static void RestoreOpacity(TextBlock textBlock)
    {
        if (!GetHasOriginalOpacity(textBlock))
            return;

        textBlock.SetCurrentValue(
            UIElement.OpacityProperty,
            GetOriginalOpacity(textBlock));
    }
}