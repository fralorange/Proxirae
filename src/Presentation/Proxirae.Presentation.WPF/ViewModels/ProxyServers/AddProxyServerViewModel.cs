using CommunityToolkit.Mvvm.Input;
using Proxirae.Application.Factories.Proxy;
using Proxirae.Application.Security.Protection;
using Proxirae.Contracts.DTOs.Proxies;

namespace Proxirae.Presentation.WPF.ViewModels.ProxyServers
{
    public partial class AddProxyServerViewModel : ProxyServerBaseViewModel
    {
        public ProxyAddDto? ProxyServer { get; private set; }

        public AddProxyServerViewModel(IProtector protector) : base(protector) { }

        [RelayCommand]
        private void Confirm()
        {
            if (!TryPrepareCredentials(out var normalizedUsername, out var protectedPassword))
                return;

            ProxyServer = ProxyFactory.CreateAddDto(
                Address,
                Port!.Value,
                Protocol,
                normalizedUsername,
                protectedPassword,
                Remarks);

            DialogResult = true;
        }
    }
}
