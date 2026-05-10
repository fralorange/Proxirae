using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using MvvmDialogs;
using Proxirae.Contracts.DTOs.Proxies;
using System.ComponentModel.DataAnnotations;

namespace Proxirae.Presentation.WPF.ViewModels.ProxyChecker
{
    public partial class SetProxyServerViewModel : ObservableValidator, IModalDialogViewModel
    {
        private bool? _dialogResult;

        public bool? DialogResult
        {
            get => _dialogResult;
            protected set => SetProperty(ref _dialogResult, value);
        }

        [ObservableProperty]
        [Required]
        private string _address = string.Empty;

        [ObservableProperty]
        [Required]
        [Range(1, 65535)]
        private int? _port;

        [ObservableProperty]
        [Required]
        private ProxyTypeDto _protocol = ProxyTypeDto.SOCKS5;

        [ObservableProperty]
        private string? _username;

        [ObservableProperty]
        private string? _password;

        [ObservableProperty]
        private bool _isAuth;

        public ProxyDetailDto? ProxyServer { get; private set; }

        public SetProxyServerViewModel(ProxyDetailDto? proxy)
        {
            if (proxy is not null)
            {
                Address = proxy.IP;
                Port = proxy.Port;
                Protocol = proxy.Type;
                Username = proxy.Username;
                Password = proxy.Password;
            }
        }

        [RelayCommand]
        private void Confirm()
        {
            ValidateAllProperties();

            if (HasErrors)
                return;

            ProxyServer = new ProxyDetailDto
            {
                IP = Address,
                Port = Port!.Value,
                Type = Protocol,
                Username = Username,
                Password = Password,
            };

            DialogResult = true;
        }
    }
}
