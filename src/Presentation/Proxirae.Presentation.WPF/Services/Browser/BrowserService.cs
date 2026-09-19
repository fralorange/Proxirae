using Proxirae.Application.Services.Browser;

namespace Proxirae.Presentation.WPF.Services.Browser
{
    public class BrowserService : IBrowserService
    {
        public void OpenUrl(string url)
        {
            if (string.IsNullOrEmpty(url)) return;

            System.Diagnostics.Process.Start(new System.Diagnostics.ProcessStartInfo
            {
                FileName = url,
                UseShellExecute = true,
            });
        }
    }
}
