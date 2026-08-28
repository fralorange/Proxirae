using CommunityToolkit.Mvvm.Input;
using Proxirae.Application.Factories.Proxy;
using Proxirae.Contracts.DTOs.Proxies;

namespace Proxirae.Presentation.WPF.ViewModels.ProxyServers
{
    public partial class AddProxyServerViewModel : ProxyServerBaseViewModel
    {
        public ProxyAddDto? ProxyServer { get; private set; }

        [RelayCommand]
        private void Confirm()
        {
            ValidateAllProperties();

            if (HasErrors)
                return;

            ProxyServer = ProxyFactory.CreateAddDto(Address, Port!.Value, Protocol, Username, Password, Remarks);

            DialogResult = true;
        }
    }
}
