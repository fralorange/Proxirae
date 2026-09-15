using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using MvvmDialogs;
using Proxirae.Application.Services.Browser;

namespace Proxirae.Presentation.WPF.ViewModels.About
{
    public partial class AboutViewModel : ObservableObject, IModalDialogViewModel
    {
        private readonly IBrowserService _browserService;

        public string AppName => ProgramInfo.AppName;

        public string AppVersion =>
            $"Version {ProgramInfo.AppVersion} {ProgramInfo.AppBitness}";

        private bool? _dialogResult;
        public bool? DialogResult
        {
            get => _dialogResult;
            protected set => SetProperty(ref _dialogResult, value);
        }

        public AboutViewModel(IBrowserService browserService)
        {
            _browserService = browserService;
        }

        [RelayCommand]
        private void OpenLink(string url)
        {
            _browserService.OpenUrl(url);
        }
    }
}
