using CommunityToolkit.Mvvm.Input;
using Proxirae.Contracts.DTOs.Proxies;

namespace Proxirae.Presentation.WPF.ViewModels.ProxyServers
{
    public partial class AddProxyServerViewModel : ProxyServerBaseViewModel
    {
        public ProxyAddDto? ProxyServer { get; private set; }

        // TODO: Extract the creation of ProxyAddDto to the factory
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
