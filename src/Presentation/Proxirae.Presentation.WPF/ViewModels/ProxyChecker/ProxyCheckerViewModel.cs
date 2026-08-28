using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using MvvmDialogs;
using Proxirae.Application.Factories.Proxy;
using Proxirae.Application.Services.Test;
using Proxirae.Contracts.DTOs.Proxies;
using Proxirae.Contracts.DTOs.Test;
using Proxirae.Presentation.WPF.Facades.Dialog;

namespace Proxirae.Presentation.WPF.ViewModels.ProxyChecker
{
    public partial class ProxyCheckerViewModel : ObservableObject, IModalDialogViewModel, IDisposable
    {
        private readonly DialogFacade _dialogFacade;
        private readonly ITestService _testService;

        private bool? _dialogResult;
        public bool? DialogResult
        {
            get => _dialogResult;
            protected set => SetProperty(ref _dialogResult, value);
        }

        private double _totalLatency = 0;
        private Guid _currentTestId = Guid.Empty;

        [ObservableProperty]
        [NotifyPropertyChangedFor(nameof(IsProxySet))]
        [NotifyCanExecuteChangedFor(nameof(TestCommand))]
        private ProxyTestDto? _proxyServer;

        [ObservableProperty]
        private string _testAddress = "www.google.com";

        [ObservableProperty]
        private ushort _testPort = 80;

        [ObservableProperty]
        private int _progressValue;

        [ObservableProperty]
        private string _statusMessage = "Idle";

        [ObservableProperty]
        private string _latencyMessage = string.Empty;

        [ObservableProperty]
        private bool _isSuccess;

        [ObservableProperty]
        private bool _isFailed;

        [ObservableProperty]
        [NotifyCanExecuteChangedFor(nameof(TestCommand))]
        private bool _isTesting;

        public bool IsProxySet => ProxyServer is not null;

        public bool CanExecuteTest => IsProxySet && !IsTesting;

        public ProxyCheckerViewModel(DialogFacade dialogFacade, ITestService testService)
        {
            _dialogFacade = dialogFacade;
            _testService = testService;

            _testService.ProgressReceived += OnProgressReceived;
        }

        public void Initialize(ProxyDetailDto proxy)
        {
            ProxyServer = ProxyFactory.CreateTestDto(proxy.Address, proxy.Port, proxy.Type, proxy.Username, proxy.Password);
        }

        private void OnProgressReceived(TestProgressDto progress)
        {
            if (progress.Id != _currentTestId)
                return;

            if (progress.Latency.HasValue)
            {
                _totalLatency += progress.Latency.Value;
                LatencyMessage = $"{Math.Round(_totalLatency, 2)} ms";
            }

            switch (progress.Stage)
            {
                case TestProgressStage.Establish:
                    ProgressValue = 1;
                    StatusMessage = "Establishing connection...";
                    break;

                case TestProgressStage.Handshake:
                    ProgressValue = 2;
                    StatusMessage = "Performing handshake...";
                    break;

                case TestProgressStage.Connect:
                    ProgressValue = 3;
                    IsSuccess = true;
                    StatusMessage = "Connected successfully!";
                    break;

                case TestProgressStage.Failed:
                    IsFailed = true;
                    StatusMessage = "Connection failed.";
                    break;
            }
        }

        [RelayCommand]
        private void SetProxyServer()
        {
            var viewModel = new SetProxyServerViewModel(ProxyServer);
            _dialogFacade.ShowDialog(this, viewModel);

            if (viewModel.ProxyServer is not null)
            {
                ProxyServer = viewModel.ProxyServer;
            }
        }

        [RelayCommand]
        private async Task Test(CancellationToken cancellationToken)
        {
            if (ProxyServer is null)
                return;

            _totalLatency = 0;
            _currentTestId = Guid.NewGuid();

            ProgressValue = 0;
            IsSuccess = false;
            IsFailed = false;
            LatencyMessage = string.Empty;
            StatusMessage = "Starting test...";
            IsTesting = true;

            var testDto = new TestDto
            {
                Id = _currentTestId,
                Proxy = ProxyServer,
                TestAddress = TestAddress,
                TestPort = TestPort
            };

            try
            {
                await _testService.InitiateTestAsync(testDto, cancellationToken);
            }
            finally
            {
                IsTesting = false;
            }
        }

        public void Dispose()
        {
            _testService.ProgressReceived -= OnProgressReceived;
        }
    }
}
