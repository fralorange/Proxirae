using Microsoft.Extensions.DependencyInjection;
using MvvmDialogs;
using System.ComponentModel;

namespace Proxirae.Presentation.WPF.Facades.Dialog
{
    public class DialogFacade
    {
        private readonly IDialogService _dialogService;
        private readonly IServiceProvider _serviceProvider;

        public DialogFacade(IDialogService dialogService, IServiceProvider serviceProvider)
        {
            _dialogService = dialogService;
            _serviceProvider = serviceProvider;
        }

        public TViewModel ShowDialog<TViewModel>(INotifyPropertyChanged ownerViewModel) where TViewModel : IModalDialogViewModel
        {
            var viewModel = _serviceProvider.GetRequiredService<TViewModel>();
            _dialogService.ShowDialog(ownerViewModel, viewModel);
            return viewModel;
        }

        public void ShowDialog<TViewModel>(INotifyPropertyChanged ownerViewModel, TViewModel viewModel) where TViewModel : IModalDialogViewModel
        {
            _dialogService.ShowDialog(ownerViewModel, viewModel);
        }
    }
}
