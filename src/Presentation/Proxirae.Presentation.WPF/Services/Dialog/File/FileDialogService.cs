using MvvmDialogs;
using MvvmDialogs.FrameworkDialogs.OpenFile;
using MvvmDialogs.FrameworkDialogs.SaveFile;
using System.ComponentModel;

namespace Proxirae.Presentation.WPF.Services.Dialog.File
{
    public class FileDialogService : IFileDialogService
    {
        private readonly IDialogService _dialogService;

        public FileDialogService(IDialogService dialogService)
        {
            _dialogService = dialogService;
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
