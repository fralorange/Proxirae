using Microsoft.Extensions.DependencyInjection;
using MvvmDialogs;
using MvvmDialogs.FrameworkDialogs.OpenFile;
using MvvmDialogs.FrameworkDialogs.SaveFile;
using System.ComponentModel;

namespace Proxirae.Presentation.WPF.Facades.Dialog
{
    public class DialogFacade
    {
        private readonly IDialogService _dialogService;
        private readonly IServiceScopeFactory _scopeFactory;

        public DialogFacade(IDialogService dialogService, IServiceScopeFactory scopeFactory)
        {
            _dialogService = dialogService;
            _scopeFactory = scopeFactory;
        }

        public TViewModel ShowDialog<TViewModel>(INotifyPropertyChanged ownerViewModel) where TViewModel : IModalDialogViewModel
        {
            using var scope = _scopeFactory.CreateAsyncScope();

            var viewModel = scope.ServiceProvider.GetRequiredService<TViewModel>();

            _dialogService.ShowDialog(ownerViewModel, viewModel);
            return viewModel;
        }

        public void ShowDialog<TViewModel>(INotifyPropertyChanged ownerViewModel, TViewModel viewModel) where TViewModel : IModalDialogViewModel
        {
            _dialogService.ShowDialog(ownerViewModel, viewModel);
        }

        public string? OpenFile(INotifyPropertyChanged ownerViewModel, OpenFileDialogSettings settings)
        {
            var success = _dialogService.ShowOpenFileDialog(ownerViewModel, settings);
            return success == true ? settings.FileName : null;
        }

        public string? SaveFile(INotifyPropertyChanged ownerViewModel, SaveFileDialogSettings settings)
        {
            var success = _dialogService.ShowSaveFileDialog(ownerViewModel, settings);
            return success == true ? settings.FileName : null;
        }
    }
}
