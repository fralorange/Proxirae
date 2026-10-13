using MvvmDialogs;
using MvvmDialogs.FrameworkDialogs.FolderBrowser;
using System.ComponentModel;

namespace Proxirae.Presentation.WPF.Services.Dialog.Folder
{
    public class FolderDialogService : IFolderDialogService
    {
        private readonly IDialogService _dialogService;

        public FolderDialogService(IDialogService dialogService)
        {
            _dialogService = dialogService;
        }

        public string? ShowOpenFolderDialog(INotifyPropertyChanged ownerViewModel, FolderBrowserDialogSettings settings)
        {
            var success = _dialogService.ShowFolderBrowserDialog(ownerViewModel, settings);
            return success == true ? settings.SelectedPath : null;
        }
    }
}
