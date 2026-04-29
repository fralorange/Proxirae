using Proxirae.Presentation.WPF.Controls;
using Proxirae.Presentation.WPF.ViewModels;

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
    }
}