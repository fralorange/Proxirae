using Proxirae.Presentation.WPF.Services.Dialog.Modal;
using Proxirae.Presentation.WPF.ViewModels.Messages;
using System.ComponentModel;

namespace Proxirae.Presentation.WPF.Services.Dialog.Message
{
    public class MessageDialogService : IMessageDialogService
    {
        private readonly IModalDialogService _modalDialogService;

        public MessageDialogService(IModalDialogService modalDialogService)
        {
            _modalDialogService = modalDialogService;
        }

        public void ShowError(INotifyPropertyChanged ownerViewModel, string message, string title)
        {
            var errorViewModel = new ErrorViewModel(message, title);

            _modalDialogService.ShowDialog(ownerViewModel, errorViewModel);
        }

        public bool ShowWarning(INotifyPropertyChanged ownerViewModel, string message, string title)
        {
            var warningViewModel = new WarningViewModel(message, title);

            _modalDialogService.ShowDialog(ownerViewModel, warningViewModel);

            return warningViewModel.DialogResult == true;
        }

        public MessageDialogResult ShowConfirmation(INotifyPropertyChanged ownerViewModel, string message, string title, bool showCancel = false)
        {
            var confirmationViewModel = new ConfirmationViewModel(message, title, showCancel);

            _modalDialogService.ShowDialog(ownerViewModel, confirmationViewModel);

            return confirmationViewModel.Result;
        }
    }
}
