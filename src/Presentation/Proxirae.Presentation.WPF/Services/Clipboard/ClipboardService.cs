using Proxirae.Application.Services.Clipboard;

using WpfClipboard = System.Windows.Clipboard;

namespace Proxirae.Presentation.WPF.Services.Clipboard
{
    public class ClipboardService : IClipboardService
    {
        public void Copy(string text)
        {
            if (!string.IsNullOrEmpty(text))
            {
                WpfClipboard.SetText(text);
            }
        }
    }
}
