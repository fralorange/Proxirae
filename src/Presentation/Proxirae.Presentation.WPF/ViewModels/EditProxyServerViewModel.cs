using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using Proxirae.Contracts.DTOs.Proxy;

namespace Proxirae.Presentation.WPF.ViewModels
{
    public partial class EditProxyServerViewModel : ProxyServerBaseViewModel
    {
        private readonly Guid _id;

        [ObservableProperty]
        private ProxyEditDto? _proxyServer;

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
