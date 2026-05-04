using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using Proxirae.Contracts.DTOs.Proxy;

namespace Proxirae.Presentation.WPF.ViewModels
{
    public partial class AddProxyServerViewModel : ProxyServerBaseViewModel
    {
        [ObservableProperty]
        private ProxyAddDto? _proxyServer;

        [RelayCommand]
        private void Confirm()
        {
            ValidateAllProperties();

            if (HasErrors)
                return;

            ProxyServer = new ProxyAddDto
            {
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
