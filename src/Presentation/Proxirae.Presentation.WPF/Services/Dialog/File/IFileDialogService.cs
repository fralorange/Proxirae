using MvvmDialogs.FrameworkDialogs.OpenFile;
using MvvmDialogs.FrameworkDialogs.SaveFile;
using System.ComponentModel;

namespace Proxirae.Presentation.WPF.Services.Dialog.File
{
    public interface IFileDialogService
    {
        string? OpenFile(INotifyPropertyChanged ownerViewModel, OpenFileDialogSettings settings);
        string? SaveFile(INotifyPropertyChanged ownerViewModel, SaveFileDialogSettings settings);
    }
}
