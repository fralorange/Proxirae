using Proxirae.Presentation.WPF.Services.Dialog.Modal;
using Proxirae.Presentation.WPF.ViewModels.Input;
using System.ComponentModel;

namespace Proxirae.Presentation.WPF.Services.Dialog.Input
{
    public class InputDialogService : IInputDialogService
    {
        private readonly IModalDialogService _modalDialogService;

        public InputDialogService(IModalDialogService modalDialogService)
        {
            _modalDialogService = modalDialogService;
        }

        public InputDialogResponse<string> ShowText(INotifyPropertyChanged ownerViewModel, string message, string title, string? alternativeBtnText = null)
        {
            var viewModel = new TextInputViewModel(message, title, alternativeBtnText);

            _modalDialogService.ShowDialog(ownerViewModel, viewModel);

            return new(viewModel.Input, viewModel.Result);
        }
    }
}
