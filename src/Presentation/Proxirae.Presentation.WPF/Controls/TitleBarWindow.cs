using Proxirae.Presentation.WPF.Controls.Helpers;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Input;
using System.Windows.Interop;
using System.Windows.Markup;
using System.Windows.Media;
using System.Windows.Shapes;
using System.Windows.Shell;
using ApplicationEnt = System.Windows.Application;

namespace Proxirae.Presentation.WPF.Controls
{
    /// <summary>
    /// A class that derives from <see cref="Window"/>.
    /// This class represents window with custom title bar.
    /// </summary>
    [ContentProperty(nameof(WindowContent))]
    public class TitleBarWindow : Window
    {
        private static readonly Brush TitleBarBackgroundBrush = Brushes.WhiteSmoke;
        private static readonly Brush TitleBarButtonBackgroundBrush = Brushes.Transparent;
        private static readonly Brush TitleBarButtonHoverBackgroundBrush = new SolidColorBrush(Color.FromRgb(233, 236, 239));
        private static readonly Brush TitleBarButtonPressedBackgroundBrush = new SolidColorBrush(Color.FromRgb(221, 226, 230));
        private static readonly Brush TitleBarButtonForegroundBrush = new SolidColorBrush(Color.FromRgb(32, 33, 36));
        private static readonly Geometry MaximizeIconGeometry = Geometry.Parse("M 13.5,10.5 H 22.5 V 19.5 H 13.5 Z");
        private static readonly Geometry RestoreIconGeometry = Geometry.Parse("M 13.5,12.5 H 20.5 V 19.5 H 13.5 Z M 15.5,12.5 V 10.5 H 22.5 V 17.5 H 20.5");

        private Button maximizeRestoreButton;
        private Path maximizeRestoreIcon;
        private Grid titleBar;
        private WindowChrome windowChrome;

        /// <summary>
        /// A title bar menu content dependency property.
        /// </summary>
        public static readonly DependencyProperty TitleBarMenuContentProperty =
            DependencyProperty.Register(
                nameof(TitleBarMenuContent),
                typeof(object),
                typeof(TitleBarWindow),
                new PropertyMetadata(null, OnTitleBarMenuContentChanged));

        /// <summary>
        /// A window content dependency property.
        /// </summary>
        public static readonly DependencyProperty WindowContentProperty =
            DependencyProperty.Register(nameof(WindowContent), typeof(object), typeof(TitleBarWindow));

        /// <summary>
        /// A center title parameter dependency property.
        /// </summary>
        public static readonly DependencyProperty ShowCenteredTitleProperty =
            DependencyProperty.Register(
                nameof(ShowCenteredTitle),
                typeof(bool),
                typeof(TitleBarWindow),
                new PropertyMetadata(false));

        /// <summary>
        /// A minimize button parameter dependency property.
        /// </summary>
        public static readonly DependencyProperty ShowMinimizeButtonProperty =
            DependencyProperty.Register(
                nameof(ShowMinimizeButton),
                typeof(bool),
                typeof(TitleBarWindow),
                new PropertyMetadata(true));

        /// <summary>
        /// A maximize button parameter dependency property.
        /// </summary>
        public static readonly DependencyProperty ShowMaximizeButtonProperty =
            DependencyProperty.Register(
                nameof(ShowMaximizeButton),
                typeof(bool),
                typeof(TitleBarWindow),
                new PropertyMetadata(true));

        /// <summary>
        /// A dialog parameter dependency property.
        /// </summary>
        public static readonly DependencyProperty IsDialogProperty =
            DependencyProperty.Register(
                nameof(IsDialog),
                typeof(bool),
                typeof(TitleBarWindow),
                new PropertyMetadata(false));

        /// <summary>
        /// Gets or sets title bar menu content.
        /// </summary>
        public object TitleBarMenuContent
        {
            get => GetValue(TitleBarMenuContentProperty);
            set => SetValue(TitleBarMenuContentProperty, value);
        }

        /// <summary>
        /// Gets or sets window content.
        /// </summary>
        public object WindowContent
        {
            get => GetValue(WindowContentProperty);
            set => SetValue(WindowContentProperty, value);
        }

        /// <summary>
        /// Gets or sets 'show centered title' parameter.
        /// </summary>
        public bool ShowCenteredTitle
        {
            get => (bool)GetValue(ShowCenteredTitleProperty);
            set => SetValue(ShowCenteredTitleProperty, value);
        }

        /// <summary>
        /// Gets or sets 'show minimize button' parameter.
        /// </summary>
        public bool ShowMinimizeButton
        {
            get => (bool)GetValue(ShowMinimizeButtonProperty);
            set => SetValue(ShowMinimizeButtonProperty, value);
        }

        /// <summary>
        /// Gets or sets 'show maximize button' parameter.
        /// </summary>
        public bool ShowMaximizeButton
        {
            get => (bool)GetValue(ShowMaximizeButtonProperty);
            set => SetValue(ShowMaximizeButtonProperty, value);
        }

        /// <summary>
        /// Gets or sets 'is dialog' parameter.
        /// </summary>
        public bool IsDialog
        {
            get => (bool)GetValue(IsDialogProperty);
            set => SetValue(IsDialogProperty, value);
        }

        /// <summary>
        /// Initializes title bar window.
        /// </summary>
#pragma warning disable CS8618
        public TitleBarWindow()
        {
            InitializeComponents();

            InitializeWindowChrome();

            ResizeMode = ResizeMode.CanResize;
            WindowStyle = WindowStyle.SingleBorderWindow;
            SnapsToDevicePixels = true;

            StateChanged += OnWindowStateChanged;
        }
#pragma warning restore CS8618

        private void InitializeWindowChrome()
        {
            windowChrome = new WindowChrome
            {
                CornerRadius = new CornerRadius(0),
                GlassFrameThickness = new Thickness(0),
                NonClientFrameEdges = NonClientFrameEdges.None,
                ResizeBorderThickness = new Thickness(5),
                UseAeroCaptionButtons = false
            };

            Loaded += (s, e) =>
            {
                if (IsDialog)
                {
                    windowChrome.GlassFrameThickness = new Thickness(0.1, 0, 0.1, 0.1);
                    windowChrome.NonClientFrameEdges = NonClientFrameEdges.Left | NonClientFrameEdges.Right | NonClientFrameEdges.Bottom;
                }
            };

            titleBar.SizeChanged += OnTitleBarSizeChanged;
            UpdateCaptionHeight();

            WindowChrome.SetWindowChrome(this, windowChrome);
        }

        private void InitializeComponents()
        {
            // Main container
            var dockPanel = new DockPanel();
            Content = dockPanel;

            // Create title bar
            CreateTitleBar(dockPanel);

            // Window content
            var contentPresenter = new ContentPresenter();
            contentPresenter.SetBinding(ContentPresenter.ContentProperty,
                new Binding("WindowContent") { Source = this });

            dockPanel.Children.Add(contentPresenter);
        }

        private void CreateTitleBar(DockPanel dockPanel)
        {
            titleBar = new Grid
            {
                Focusable = false,
                MinHeight = GetResource<double>("TitleBarHeight"),
                ColumnDefinitions =
                {
                    new ColumnDefinition { Width = GridLength.Auto },
                    new ColumnDefinition { Width = new GridLength(1, GridUnitType.Star), MinWidth = 40 },
                    new ColumnDefinition { Width = GridLength.Auto },
                    new ColumnDefinition { Width = GridLength.Auto },
                    new ColumnDefinition { Width = GridLength.Auto }
                }
            };
            titleBar.Background = TitleBarBackgroundBrush;
            DockPanel.SetDock(titleBar, Dock.Top);
            dockPanel.Children.Add(titleBar);
            WindowChrome.SetIsHitTestVisibleInChrome(titleBar, false);

            // Window icon
            CreateIcon();

            // Menu and title
            CreateMenuSection();

            // Control buttons
            CreateMinimizeButton();
            CreateMaximizeButton();
            CreateCloseButton();
        }

        private void CreateIcon()
        {
            var icon = new Image
            {
                Focusable = false,
                Style = GetResource<Style>("IconImage"),
            };
            icon.SetBinding(Image.SourceProperty, new Binding(nameof(Icon)) { Source = this });
            icon.MouseDown += OnIconMouseDown;
            titleBar.Children.Add(icon);
            Grid.SetColumn(icon, 0);
            WindowChrome.SetIsHitTestVisibleInChrome(icon, true);
        }

        private void CreateMenuSection()
        {
            var dockPanelInner = new DockPanel
            {
                Focusable = false,
                HorizontalAlignment = HorizontalAlignment.Stretch
            };
            Grid.SetColumn(dockPanelInner, 1);
            titleBar.Children.Add(dockPanelInner);

            // Menu presenter
            var menuPresenter = new ContentPresenter
            {
                Focusable = false,
                VerticalAlignment = VerticalAlignment.Center
            };
            menuPresenter.SetBinding(ContentPresenter.ContentProperty,
                new Binding("TitleBarMenuContent") { Source = this });
            DockPanel.SetDock(menuPresenter, Dock.Left);
            WindowChrome.SetIsHitTestVisibleInChrome(menuPresenter, true);
            dockPanelInner.Children.Add(menuPresenter);

            // Centered Title (hidden by default)
            var titleTextBlock = new TextBlock
            {
                Focusable = false,
                Margin = new Thickness(10, 0, 10, 0),
                HorizontalAlignment = HorizontalAlignment.Center,
                VerticalAlignment = VerticalAlignment.Center,
                TextTrimming = TextTrimming.CharacterEllipsis,
                TextWrapping = TextWrapping.NoWrap
            };

            // Bind Text to Window.Title
            titleTextBlock.SetBinding(TextBlock.TextProperty,
                new Binding("Title") { Source = this });

            // Bind Visibility to ShowCenteredTitle with converter
            titleTextBlock.SetBinding(TextBlock.VisibilityProperty,
                new Binding(nameof(ShowCenteredTitle))
                {
                    Source = this,
                    Converter = new BooleanToVisibilityConverter()
                });

            dockPanelInner.Children.Add(titleTextBlock);
        }

        private void CreateMinimizeButton()
        {
            var button = new Button
            {
                Focusable = false,
                IsTabStop = false,
                Style = GetResource<Style>("TitleBarButtonStyle"),
                ToolTip = "Minimize"
            };

            button.SetBinding(VisibilityProperty,
                new Binding(nameof(ShowMinimizeButton))
                {
                    Source = this,
                    Converter = new BooleanToVisibilityConverter()
                });

            button.Click += OnMinimizeButtonClick;

            var path = new Path
            {
                Focusable = false,
                Width = 36,
                Height = 32,
                Data = Geometry.Parse("M 13,15 H 23"),
                StrokeThickness = 1
            };
            path.SetBinding(Shape.StrokeProperty, new Binding(nameof(Button.Foreground)) { Source = button });
            button.Content = path;

            titleBar.Children.Add(button);
            Grid.SetColumn(button, 2);
        }

        private void CreateMaximizeButton()
        {
            maximizeRestoreButton = new Button
            {
                Focusable = false,
                IsTabStop = false,
                Style = GetResource<Style>("TitleBarButtonStyle"),
                ToolTip = "Maximize"
            };

            maximizeRestoreButton.SetBinding(VisibilityProperty,
                new Binding(nameof(ShowMaximizeButton))
                {
                    Source = this,
                    Converter = new BooleanToVisibilityConverter()
                });

            maximizeRestoreButton.Click += OnMaximizeRestoreButtonClick;
            maximizeRestoreButton.ToolTipOpening += OnMaximizeRestoreButtonToolTipOpening;

            maximizeRestoreIcon = new Path
            {
                Focusable = false,
                Width = 36,
                Height = 32,
                StrokeThickness = 1,
                Data = GetMaximizeRestoreIconGeometry()
            };
            maximizeRestoreIcon.SetBinding(Shape.StrokeProperty, new Binding(nameof(Button.Foreground)) { Source = maximizeRestoreButton });

            maximizeRestoreButton.Content = maximizeRestoreIcon;
            titleBar.Children.Add(maximizeRestoreButton);
            Grid.SetColumn(maximizeRestoreButton, 3);
        }

        private void CreateCloseButton()
        {
            var button = new Button
            {
                Focusable = false,
                IsTabStop = false,
                Style = GetResource<Style>("TitleBarCloseButtonStyle"),
                ToolTip = "Close"
            };
            button.Click += OnCloseButtonClick;

            var path = new Path
            {
                Focusable = false,
                Width = 36,
                Height = 32,
                Data = Geometry.Parse("M 13,11 22,20 M 13,20 22,11"),
                StrokeThickness = 1
            };
            path.SetBinding(Shape.StrokeProperty, new Binding(nameof(Button.Foreground)) { Source = button });
            button.Content = path;

            titleBar.Children.Add(button);
            Grid.SetColumn(button, 4);
        }

        private static T GetResource<T>(string key)
        {
            if (ApplicationEnt.Current.Resources.Contains(key))
            {
                var resource = ApplicationEnt.Current.Resources[key];
                if (resource is T typedResource)
                {
                    return typedResource;
                }
                throw new InvalidCastException($"Resource '{key}' is not of type {typeof(T)}");
            }
            throw new ArgumentException($"Resource '{key}' not found");
        }

        // Event handlers
        private void OnMinimizeButtonClick(object sender, RoutedEventArgs e) => SystemCommands.MinimizeWindow(this);
        private void OnMaximizeRestoreButtonClick(object sender, RoutedEventArgs e) => ToggleWindowState();
        private void OnCloseButtonClick(object sender, RoutedEventArgs e) => SystemCommands.CloseWindow(this);
        private void OnTitleBarSizeChanged(object sender, SizeChangedEventArgs e) => UpdateCaptionHeight();

        protected override void OnPropertyChanged(DependencyPropertyChangedEventArgs e)
        {
            base.OnPropertyChanged(e);

            if (e.Property == BorderThicknessProperty)
            {
                UpdateCaptionHeight();
            }
        }

        private void UpdateCaptionHeight()
        {
            if (windowChrome is null || titleBar is null)
            {
                return;
            }

            windowChrome.CaptionHeight = titleBar.ActualHeight + BorderThickness.Top;
        }

        private Geometry GetMaximizeRestoreIconGeometry()
        {
            return WindowState == WindowState.Normal
                ? MaximizeIconGeometry
                : RestoreIconGeometry;
        }

        private void OnIconMouseDown(object sender, MouseButtonEventArgs e)
        {
            if (e.ClickCount == 2 && e.ChangedButton == MouseButton.Left)
            {
                Close();
            }
            else if (e.ChangedButton == MouseButton.Left || e.ChangedButton == MouseButton.Right)
            {
                ShowSystemMenu(this, e.GetPosition(this));
            }
        }

        private static void ShowSystemMenu(Window window, Point point)
        {
            // Increment coordinates to allow double-click
            ++point.X;
            ++point.Y;
            if (window.WindowState == WindowState.Normal)
            {
                point.X += window.Left;
                point.Y += window.Top;
            }
            SystemCommands.ShowSystemMenu(window, point);
        }

        private void ToggleWindowState()
        {
            WindowState = WindowState == WindowState.Maximized
                ? WindowState.Normal
                : WindowState.Maximized;
        }

        private void OnWindowStateChanged(object? sender, EventArgs e)
        {
            var content = (WindowContent as FrameworkElement);
            maximizeRestoreIcon.Data = GetMaximizeRestoreIconGeometry();

            if (WindowState == WindowState.Maximized)
            {
                titleBar.Margin = new Thickness(6, 6, 6, 0);
                content?.SetValue(FrameworkElement.MarginProperty, new Thickness(6, 0, 6, 6));
            }
            else
            {
                titleBar.Margin = new Thickness(0);
                content?.SetValue(FrameworkElement.MarginProperty, new Thickness(0));
            }
        }

        private void OnMaximizeRestoreButtonToolTipOpening(object sender, ToolTipEventArgs e)
        {
            maximizeRestoreButton.ToolTip = WindowState == WindowState.Normal
                ? "Maximize"
                : "Restore";
        }

        private IntPtr WndProc(IntPtr hwnd, int msg, IntPtr wParam, IntPtr lParam, ref bool handled)
        {
            switch (msg)
            {
                case NativeHelper.WM_NCHITTEST:
                    if (NativeHelper.IsSnapLayoutEnabled())
                    {
                        // Return HTMAXBUTTON when the mouse is over the maximize/restore button
                        var point = PointFromScreen(new Point(lParam.ToInt32() & 0xFFFF, lParam.ToInt32() >> 16));
                        if (WpfHelper.GetElementBoundsRelativeToWindow(maximizeRestoreButton, this).Contains(point))
                        {
                            handled = true;
                            ApplyMaximizeRestoreButtonColors(TitleBarButtonHoverBackgroundBrush);
                            return new IntPtr(NativeHelper.HTMAXBUTTON);
                        }
                        else
                        {
                            ApplyMaximizeRestoreButtonColors(TitleBarButtonBackgroundBrush);
                        }
                    }
                    break;
                case NativeHelper.WM_NCLBUTTONDOWN:
                    if (NativeHelper.IsSnapLayoutEnabled())
                    {
                        if (wParam.ToInt32() == NativeHelper.HTMAXBUTTON)
                        {
                            handled = true;
                            ApplyMaximizeRestoreButtonColors(TitleBarButtonPressedBackgroundBrush);
                        }
                    }
                    break;
                case NativeHelper.WM_NCLBUTTONUP:
                    if (NativeHelper.IsSnapLayoutEnabled())
                    {
                        if (wParam.ToInt32() == NativeHelper.HTMAXBUTTON)
                        {
                            ApplyMaximizeRestoreButtonColors(TitleBarButtonBackgroundBrush);
                            ToggleWindowState();
                        }
                    }
                    break;
            }
            return IntPtr.Zero;
        }

        private void ApplyMaximizeRestoreButtonColors(Brush background)
        {
            maximizeRestoreButton.Background = background;
            maximizeRestoreButton.Foreground = TitleBarButtonForegroundBrush;
        }

        private static void OnTitleBarMenuContentChanged(DependencyObject dependencyObject, DependencyPropertyChangedEventArgs e)
        {
            DisableTitleBarContentFocus(e.NewValue);
        }

        private static void DisableTitleBarContentFocus(object content)
        {
            if (content is UIElement uiElement)
            {
                uiElement.Focusable = false;
            }

            if (content is Control control)
            {
                control.IsTabStop = false;
            }
        }

        /// <inheritdoc/>
        protected override void OnSourceInitialized(EventArgs e)
        {
            base.OnSourceInitialized(e);

            var source = (HwndSource)PresentationSource.FromVisual(this);
            source.AddHook(WndProc);
        }
    }
}
