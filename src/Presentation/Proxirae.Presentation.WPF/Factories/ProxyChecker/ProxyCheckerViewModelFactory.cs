using Microsoft.Extensions.DependencyInjection;
using Proxirae.Contracts.DTOs.Proxies;
using Proxirae.Presentation.WPF.ViewModels.ProxyChecker;

namespace Proxirae.Presentation.WPF.Factories.ProxyChecker
{
    public class ProxyCheckerViewModelFactory : IProxyCheckerViewModelFactory
    {
        private readonly IServiceProvider _serviceProvider;

        public ProxyCheckerViewModelFactory(IServiceProvider serviceProvider)
        {
            _serviceProvider = serviceProvider;
        }

        public ProxyCheckerViewModel Create(ProxyDetailDto detail)
        {
            var viewModel =  _serviceProvider.GetRequiredService<ProxyCheckerViewModel>();
            viewModel.Initialize(detail);

            return viewModel;
        }
    }
}
