using Proxirae.Contracts.DTOs.Proxies;
using Proxirae.Presentation.WPF.ViewModels.ProxyChecker;

namespace Proxirae.Presentation.WPF.Factories.ProxyChecker
{
    public interface IProxyCheckerViewModelFactory
    {
        ProxyCheckerViewModel Create(ProxyDetailDto detail);
    }
}
