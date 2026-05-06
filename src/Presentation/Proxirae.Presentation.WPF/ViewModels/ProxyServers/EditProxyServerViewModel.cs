using CommunityToolkit.Mvvm.Input;
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
            Address = proxy.IP;
            Port = proxy.Port;
            Protocol = proxy.Type;
            Username = proxy.Username;
            Password = proxy.Password;
        }

        // TODO: Extract the creation of ProxyEditDto to the factory
        [RelayCommand]
        private void Confirm()
        {
            ValidateAllProperties();

            if (HasErrors)
                return;

            ProxyServer = new ProxyEditDto
            {
                Id = _id,
                IP = Address,
                Port = Port!.Value,
                Type = Protocol,
                Username = Username,
                Password = Password,
                Remarks = Remarks
            };

            DialogResult = true;
        }
    }
}
