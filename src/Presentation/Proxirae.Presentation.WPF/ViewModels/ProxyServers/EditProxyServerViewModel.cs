using CommunityToolkit.Mvvm.Input;
using Proxirae.Application.Factories.Proxy;
using Proxirae.Application.Security.Protection;
using Proxirae.Contracts.DTOs.Proxies;

namespace Proxirae.Presentation.WPF.ViewModels.ProxyServers
{
    public partial class EditProxyServerViewModel : ProxyServerBaseViewModel
    {
        private readonly Guid _id;

        public ProxyEditDto? ProxyServer { get; private set; }

        public EditProxyServerViewModel(IProtector protector, ProxyDetailDto proxy) : base(protector)
        {
            _id = proxy.Id;
            Remarks = proxy.Remarks;
            Address = proxy.Address;
            Port = proxy.Port;
            Protocol = proxy.Type;
            Username = proxy.Username;

            if (proxy.Password is not null && _protector.TryUnprotect(proxy.Password, out var unprotectedPassword))
            {
                Password = unprotectedPassword;
            }
            else
            {
                Password = proxy.Password;
            }
        }

        [RelayCommand]
        private void Confirm()
        {
            if (!TryPrepareCredentials(out var normalizedUsername, out var protectedPassword))
                return;

            ProxyServer = ProxyFactory.CreateEditDto(_id, Address, Port!.Value, Protocol, normalizedUsername, protectedPassword, Remarks);

            DialogResult = true;
        }
    }
}
