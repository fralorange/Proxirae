using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using MvvmDialogs;
using Proxirae.Presentation.WPF.Services.Dialog.Message;

namespace Proxirae.Presentation.WPF.ViewModels.Messages
{
    public partial class ConfirmationViewModel : ObservableObject, IModalDialogViewModel
    {
        public string Message { get; }
        public string Title { get; }
        public bool ShowCancelButton { get; }

        public MessageDialogResult Result { get; private set; }

        private bool? _dialogResult;
        public bool? DialogResult
        {
            get => _dialogResult;
            protected set => SetProperty(ref _dialogResult, value);
        }

        public ConfirmationViewModel(string message, string title, bool showCancelButton = false)
        {
            Message = message;
            Title = title;
            ShowCancelButton = showCancelButton;
            Result = MessageDialogResult.None;
        }

        [RelayCommand]
        private void Yes()
        {
            Result = MessageDialogResult.Yes;
            DialogResult = true;
        }

        [RelayCommand]
        private void No()
        {
            Result = MessageDialogResult.No;
            DialogResult = false;
        }

        [RelayCommand]
        private void Cancel()
        {
            Result = MessageDialogResult.Cancel;
            DialogResult = false;
        }
    }
}