using System.ComponentModel;

namespace Proxirae.Presentation.WPF.Services.Dialog.Input
{
    public interface IInputDialogService
    {
        InputDialogResponse<string> ShowText(INotifyPropertyChanged ownerViewModel, string message, string title, string? alternativeBtnText = null);
    }
}
