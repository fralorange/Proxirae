using MvvmDialogs;
using System.ComponentModel;

namespace Proxirae.Presentation.WPF.Services.Dialog.Modal
{
    public interface IModalDialogService
    {
        TViewModel ShowDialog<TViewModel>(INotifyPropertyChanged ownerViewModel) where TViewModel : IModalDialogViewModel;
        void ShowDialog<TViewModel>(INotifyPropertyChanged ownerViewModel, TViewModel viewModel) where TViewModel : IModalDialogViewModel;
    }
}
