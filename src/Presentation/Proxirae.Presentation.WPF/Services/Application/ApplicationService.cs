using Proxirae.Application.Services.Application;

namespace Proxirae.Presentation.WPF.Services.Application
{
    internal class ApplicationService : IApplicationService
    {
        public void Shutdown()
        {
            WinApp.Current.Shutdown();
        }
    }
}
