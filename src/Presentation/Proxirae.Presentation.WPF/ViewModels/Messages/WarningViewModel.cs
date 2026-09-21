using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using MvvmDialogs;

namespace Proxirae.Presentation.WPF.ViewModels.Messages
{
    public partial class WarningViewModel : ObservableObject, IModalDialogViewModel
    {
        public string Message { get; }
        public string Title { get; }

        private bool? _dialogResult;
        public bool? DialogResult
        {
            get => _dialogResult;
            protected set => SetProperty(ref _dialogResult, value);
        }

        public WarningViewModel(string message, string title)
        {
            Message = message;
            Title = title;
        }

        [RelayCommand]
        private void OK()
        {
            DialogResult = true;
        }

        [RelayCommand]
        private void Cancel()
        {
            DialogResult = false;
        }
    }
}