using Microsoft.Extensions.DependencyInjection;
using MvvmDialogs;
using System.ComponentModel;

namespace Proxirae.Presentation.WPF.Services.Dialog.Modal
{
    public class ModalDialogService : IModalDialogService
    {
        private readonly IDialogService _dialogService;
        private readonly IServiceScopeFactory _scopeFactory;

        public ModalDialogService(IDialogService dialogService, IServiceScopeFactory scopeFactory)
        {
            _dialogService = dialogService;
            _scopeFactory = scopeFactory;
        }

        public TViewModel ShowDialog<TViewModel>(INotifyPropertyChanged ownerViewModel) where TViewModel : IModalDialogViewModel
        {
            using var scope = _scopeFactory.CreateScope();

            var viewModel = scope.ServiceProvider.GetRequiredService<TViewModel>();

            _dialogService.ShowDialog(ownerViewModel, viewModel);
            return viewModel;
        }

        public void ShowDialog<TViewModel>(INotifyPropertyChanged ownerViewModel, TViewModel viewModel) where TViewModel : IModalDialogViewModel
        {
            _dialogService.ShowDialog(ownerViewModel, viewModel);
        }
    }
}
