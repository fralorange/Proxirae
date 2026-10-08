using MvvmDialogs.FrameworkDialogs.OpenFile;
using MvvmDialogs.FrameworkDialogs.SaveFile;
using System.ComponentModel;

namespace Proxirae.Presentation.WPF.Services.Dialog.File
{
    public interface IFileDialogService
    {
        string? ShowOpenFileDialog(INotifyPropertyChanged ownerViewModel, OpenFileDialogSettings settings);
        string? ShowSaveFileDialog(INotifyPropertyChanged ownerViewModel, SaveFileDialogSettings settings);
    }
}
