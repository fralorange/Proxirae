namespace Proxirae.Presentation.WPF.Services.Dialog.Input
{
    public record InputDialogResponse<TValue>(TValue Value, InputDialogResult Result);
}
