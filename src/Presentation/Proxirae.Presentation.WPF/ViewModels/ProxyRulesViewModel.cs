using CommunityToolkit.Mvvm.ComponentModel;
using MvvmDialogs;

namespace Proxirae.Presentation.WPF.ViewModels
{
    public partial class ProxyRulesViewModel : ObservableObject, IModalDialogViewModel
    {
        private bool? dialogResult;

        public bool? DialogResult
        {
            get => dialogResult;
            private set => SetProperty(ref dialogResult, value);
        }
    }
}
