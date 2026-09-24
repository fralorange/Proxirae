using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using MvvmDialogs;
using Proxirae.Application.Security.Protection;
using Proxirae.Contracts.DTOs.Proxies;
using System.ComponentModel.DataAnnotations;

namespace Proxirae.Presentation.WPF.ViewModels.ProxyServers
{
    public partial class ProxyServerBaseViewModel : ObservableValidator, IModalDialogViewModel
    {
        protected readonly IProtector _protector;

        private bool? _dialogResult;

        public bool? DialogResult
        {
            get => _dialogResult;
            protected set => SetProperty(ref _dialogResult, value);
        }

        [ObservableProperty]
        private string? _remarks;

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

        public ProxyServerBaseViewModel(IProtector protector)
        {
            _protector = protector;
        }

        [RelayCommand]
        private void Cancel()
        {
            DialogResult = false;
        }

        protected bool TryPrepareCredentials(
            out string? normalizedUsername,
            out string? protectedPassword)
        {
            ValidateAllProperties();

            if (HasErrors)
            {
                normalizedUsername = null;
                protectedPassword = null;
                return false;
            }

            normalizedUsername = string.IsNullOrWhiteSpace(Username)
                ? null
                : Username.Trim();

            protectedPassword = string.IsNullOrEmpty(Password)
                ? null
                : _protector.Protect(Password);

            return true;
        }

        private void OnCredentialChanged(string? newValue)
        {
            if (newValue is not null)
            {
                IsAuth = true;
            }
        }

        partial void OnUsernameChanged(string? oldValue, string? newValue)
        {
            OnCredentialChanged(newValue);
        }

        partial void OnPasswordChanged(string? oldValue, string? newValue)
        {
            OnCredentialChanged(newValue);
        }

        partial void OnIsAuthChanged(bool oldValue, bool newValue)
        {
            if (!newValue)
            {
                Username = null;
                Password = null;
            }
        }
    }
}
