using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;

namespace Proxirae.Presentation.WPF.Controls;

public partial class TextBoxWithActions : UserControl
{
    public TextBoxWithActions()
    {
        InitializeComponent();
    }

    public static readonly DependencyProperty TextProperty =
        DependencyProperty.Register(
            nameof(Text),
            typeof(string),
            typeof(TextBoxWithActions),
            new FrameworkPropertyMetadata(
                string.Empty,
                FrameworkPropertyMetadataOptions.BindsTwoWayByDefault));

    public string Text
    {
        get => (string)GetValue(TextProperty);
        set => SetValue(TextProperty, value);
    }

    public static readonly DependencyProperty FileCommandProperty =
        DependencyProperty.Register(
            nameof(FileCommand),
            typeof(ICommand),
            typeof(TextBoxWithActions));

    public ICommand? FileCommand
    {
        get => (ICommand?)GetValue(FileCommandProperty);
        set => SetValue(FileCommandProperty, value);
    }

    public static readonly DependencyProperty FolderCommandProperty =
        DependencyProperty.Register(
            nameof(FolderCommand),
            typeof(ICommand),
            typeof(TextBoxWithActions));

    public ICommand? FolderCommand
    {
        get => (ICommand?)GetValue(FolderCommandProperty);
        set => SetValue(FolderCommandProperty, value);
    }

    public static readonly DependencyProperty FileIconPathProperty =
        DependencyProperty.Register(
            nameof(FileIconPath),
            typeof(string),
            typeof(TextBoxWithActions),
            new PropertyMetadata(string.Empty));

    public string FileIconPath
    {
        get => (string)GetValue(FileIconPathProperty);
        set => SetValue(FileIconPathProperty, value);
    }

    public static readonly DependencyProperty FolderIconPathProperty =
        DependencyProperty.Register(
            nameof(FolderIconPath),
            typeof(string),
            typeof(TextBoxWithActions),
            new PropertyMetadata(string.Empty));

    public string FolderIconPath
    {
        get => (string)GetValue(FolderIconPathProperty);
        set => SetValue(FolderIconPathProperty, value);
    }

    public static readonly DependencyProperty AcceptsReturnProperty =
    DependencyProperty.Register(
        nameof(AcceptsReturn),
        typeof(bool),
        typeof(TextBoxWithActions),
        new PropertyMetadata(false));

    public bool AcceptsReturn
    {
        get => (bool)GetValue(AcceptsReturnProperty);
        set => SetValue(AcceptsReturnProperty, value);
    }

    public static readonly DependencyProperty TextWrappingProperty =
        DependencyProperty.Register(
            nameof(TextWrapping),
            typeof(System.Windows.TextWrapping),
            typeof(TextBoxWithActions),
            new PropertyMetadata(System.Windows.TextWrapping.NoWrap));

    public System.Windows.TextWrapping TextWrapping
    {
        get => (System.Windows.TextWrapping)GetValue(TextWrappingProperty);
        set => SetValue(TextWrappingProperty, value);
    }
}