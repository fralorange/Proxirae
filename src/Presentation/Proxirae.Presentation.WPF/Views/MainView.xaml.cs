using Proxirae.Presentation.WPF.Controls;
using Proxirae.Presentation.WPF.ViewModels;
using System.ComponentModel;

namespace Proxirae.Presentation.WPF
{
    /// <summary>
    /// Interaction logic for MainView.xaml
    /// </summary>
    public partial class MainView : TitleBarWindow
    {
        public MainView(MainViewModel viewModel)
        {
            DataContext = viewModel;

            InitializeComponent();
        }

        protected override void OnClosing(CancelEventArgs e)
        {
            e.Cancel = true;
            Hide();
            base.OnClosing(e);
        }
    }
}