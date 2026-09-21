using System.ComponentModel;

namespace Proxirae.Presentation.WPF.Services.Dialog.Message
{
    public interface IMessageDialogService
    {
        void ShowError(INotifyPropertyChanged ownerViewModel, string message, string title);
        bool ShowWarning(INotifyPropertyChanged ownerViewModel, string message, string title);
        MessageDialogResult ShowConfirmation(INotifyPropertyChanged ownerViewModel, string message, string title, bool showCancel = false);
    }
}
