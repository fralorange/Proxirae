using CommunityToolkit.Mvvm.ComponentModel;
using Proxirae.Contracts.DTOs.Proxies;
using System.ComponentModel.DataAnnotations;

namespace Proxirae.Presentation.WPF.ViewModels.ProxyChecker
{
    public partial class ProxyDetailViewModel : ObservableValidator
    {
        private readonly ProxyDetailDto _proxyDetail;

        public ProxyDetailViewModel(ProxyDetailViewModel other)
            : this(new ProxyDetailDto
            {
                Id = other.Id,
                Remarks = other.Remarks,
                IP = other.IP,
                Port = other.Port,
                Type = other.Type,
                Username = other.Username,
                Password = other.Password
            })
        { }

        public ProxyDetailViewModel(ProxyDetailDto proxyDetail) 
            => _proxyDetail = proxyDetail;

        public Guid Id
        {
            get => _proxyDetail.Id;
            set => SetProperty(_proxyDetail.Id, value, _proxyDetail, (d, v) => d.Id = v);
        }

        public string? Remarks
        {
            get => _proxyDetail.Remarks;
            set => SetProperty(_proxyDetail.Remarks, value, _proxyDetail, (d, v) => d.Remarks = v);
        }

        [Required]
        public string IP
        {
            get => _proxyDetail.IP;
            set => SetProperty(_proxyDetail.IP, value, _proxyDetail, (d, v) => d.IP = v, true);
        }

        [Required]
        [Range(1, 65535)]
        public int Port
        {
            get => _proxyDetail.Port;
            set => SetProperty(_proxyDetail.Port, value, _proxyDetail, (d, v) => d.Port = v, true);
        }

        public ProxyTypeDto Type
        {
            get => _proxyDetail.Type;
            set => SetProperty(_proxyDetail.Type, value, _proxyDetail, (d, v) => d.Type = v);
        }

        public string? Username
        {
            get => _proxyDetail.Username;
            set
            {
                SetProperty(_proxyDetail.Username, value, _proxyDetail, (d, v) => d.Username = v);
                if (value is not null)
                {
                    IsAuth = true;
                }
            }
        }

        public string? Password
        {
            get => _proxyDetail.Password;
            set 
            { 
                SetProperty(_proxyDetail.Password, value, _proxyDetail, (d, v) => d.Password = v);
                if (value is not null)
                {
                    IsAuth = true;
                }
            }
        }

        [ObservableProperty]
        private bool _isAuth = false;

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
