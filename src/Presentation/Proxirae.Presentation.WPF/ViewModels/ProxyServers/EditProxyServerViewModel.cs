using CommunityToolkit.Mvvm.Input;
using Proxirae.Application.Factories.Proxy;
using Proxirae.Contracts.DTOs.Proxies;

namespace Proxirae.Presentation.WPF.ViewModels.ProxyServers
{
    public partial class EditProxyServerViewModel : ProxyServerBaseViewModel
    {
        private readonly Guid _id;

        public ProxyEditDto? ProxyServer { get; private set; }

        public EditProxyServerViewModel(ProxyDetailDto proxy)
        {
            _id = proxy.Id;
            Remarks = proxy.Remarks;
            Address = proxy.Address;
            Port = proxy.Port;
            Protocol = proxy.Type;
            Username = proxy.Username;
            Password = proxy.Password;
        }

        [RelayCommand]
        private void Confirm()
        {
            ValidateAllProperties();

            if (HasErrors)
                return;

            ProxyServer = ProxyFactory.CreateEditDto(_id, Address, Port!.Value, Protocol, Username, Password, Remarks);

            DialogResult = true;
        }
    }
}
