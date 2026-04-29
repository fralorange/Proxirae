namespace Proxirae.Presentation.WPF.Services.Dialog
{
    public interface IDialogService
    {
        void Show<TViewModel>() where TViewModel : class;
    }
}
