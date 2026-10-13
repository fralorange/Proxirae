using MvvmDialogs.FrameworkDialogs.FolderBrowser;
using System.ComponentModel;

namespace Proxirae.Presentation.WPF.Services.Dialog.Folder
{
    public interface IFolderDialogService
    {
        string? ShowOpenFolderDialog(INotifyPropertyChanged ownerViewModel, FolderBrowserDialogSettings settings);
    }
}
