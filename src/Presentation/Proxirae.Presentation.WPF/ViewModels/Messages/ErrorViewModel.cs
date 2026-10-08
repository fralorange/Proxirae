using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using MvvmDialogs;

namespace Proxirae.Presentation.WPF.ViewModels.Messages
{
    public partial class ErrorViewModel : ObservableObject, IModalDialogViewModel
    {
        public string Message { get; }
        public string Title { get; }

        private bool? _dialogResult;
        public bool? DialogResult
        {
            get => _dialogResult;
            protected set => SetProperty(ref _dialogResult, value);
        }

        public ErrorViewModel(string message, string title)
        {
            Message = message;
            Title = title;
        }

        [RelayCommand]
        private void Ok()
        {
            DialogResult = true;
        }
    }
}