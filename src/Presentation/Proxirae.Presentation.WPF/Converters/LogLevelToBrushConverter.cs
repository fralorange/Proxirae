using Proxirae.Contracts.DTOs.Logs;
using System.Windows.Media;
using System.Globalization;
using System.Windows;
using System.Windows.Data;

namespace Proxirae.Presentation.WPF.Converters
{
    public class LogLevelToBrushConverter : DependencyObject, IValueConverter
    {
        public Brush DebugBrush
        {
            get { return (Brush)GetValue(DebugBrushProperty); }
            set { SetValue(DebugBrushProperty, value); }
        }
        public static readonly DependencyProperty DebugBrushProperty =
            DependencyProperty.Register(nameof(DebugBrush), typeof(Brush), typeof(LogLevelToBrushConverter), new PropertyMetadata(Brushes.Gray));

        public Brush InfoBrush
        {
            get { return (Brush)GetValue(InfoBrushProperty); }
            set { SetValue(InfoBrushProperty, value); }
        }
        public static readonly DependencyProperty InfoBrushProperty =
            DependencyProperty.Register(nameof(InfoBrush), typeof(Brush), typeof(LogLevelToBrushConverter), new PropertyMetadata(Brushes.Green));

        public Brush WarningBrush
        {
            get { return (Brush)GetValue(WarningBrushProperty); }
            set { SetValue(WarningBrushProperty, value); }
        }
        public static readonly DependencyProperty WarningBrushProperty =
            DependencyProperty.Register(nameof(WarningBrush), typeof(Brush), typeof(LogLevelToBrushConverter), new PropertyMetadata(Brushes.Orange));

        public Brush ErrorBrush
        {
            get { return (Brush)GetValue(ErrorBrushProperty); }
            set { SetValue(ErrorBrushProperty, value); }
        }
        public static readonly DependencyProperty ErrorBrushProperty =
            DependencyProperty.Register(nameof(ErrorBrush), typeof(Brush), typeof(LogLevelToBrushConverter), new PropertyMetadata(Brushes.Red));

        public Brush CriticalBrush
        {
            get { return (Brush)GetValue(CriticalBrushProperty); }
            set { SetValue(CriticalBrushProperty, value); }
        }
        public static readonly DependencyProperty CriticalBrushProperty =
            DependencyProperty.Register(nameof(CriticalBrush), typeof(Brush), typeof(LogLevelToBrushConverter), new PropertyMetadata(Brushes.DarkRed));

        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value is LogLevelDto level)
            {
                return level switch
                {
                    LogLevelDto.Debug => DebugBrush,
                    LogLevelDto.Info => InfoBrush,
                    LogLevelDto.Warning => WarningBrush,
                    LogLevelDto.Error => ErrorBrush,
                    LogLevelDto.Critical => CriticalBrush,
                    _ => Brushes.Gray
                };
            }
            return Brushes.Gray;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return Binding.DoNothing;
        }
    }
}
