using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using MvvmDialogs;
using Proxirae.Presentation.WPF.Services.Dialog.Input;

namespace Proxirae.Presentation.WPF.ViewModels.Input
{
    public partial class TextInputViewModel : ObservableObject, IModalDialogViewModel
    {
        public string Message { get; }
        public string Title { get; }
        public string? AlternativeButtonText { get; }

        public bool ShowAlternativeButton =>
            !string.IsNullOrWhiteSpace(AlternativeButtonText);

        [ObservableProperty]
        private string _input = string.Empty;

        public InputDialogResult Result { get; private set; }

        private bool? _dialogResult;
        public bool? DialogResult
        {
            get => _dialogResult;
            protected set => SetProperty(ref _dialogResult, value);
        }

        public TextInputViewModel(string message, string title, string? alternativeBtnText = null)
        {
            Message = message;
            Title = title;
            AlternativeButtonText = alternativeBtnText;
            Result = InputDialogResult.None;
        }

        [RelayCommand]
        private void Ok()
        {
            Result = InputDialogResult.Ok;
            DialogResult = true;
        }

        [RelayCommand]
        private void Cancel()
        {
            Result = InputDialogResult.Cancel;
            DialogResult = false;
        }

        [RelayCommand]
        private void Alternative()
        {
            Result = InputDialogResult.Alternative;
            DialogResult = true;
        }
    }
}