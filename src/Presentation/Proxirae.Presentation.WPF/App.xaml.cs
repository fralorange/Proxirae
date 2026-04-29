using System.Windows;
using System.Windows.Controls;

namespace Proxirae.Presentation.WPF
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        protected override void OnStartup(StartupEventArgs e)
        {
            // Disable Tab stop globally
            Control.IsTabStopProperty.OverrideMetadata(
                typeof(Control),
                new FrameworkPropertyMetadata(false));

            base.OnStartup(e);
        }
    }

}
