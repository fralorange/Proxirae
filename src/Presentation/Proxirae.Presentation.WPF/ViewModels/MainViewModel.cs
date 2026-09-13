using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using ObservableCollections;
using Proxirae.Application.Models.Preferences.Appearance;
using Proxirae.Application.Services.Application;
using Proxirae.Application.Services.Clipboard;
using Proxirae.Application.Services.Flows;
using Proxirae.Application.Services.Logs;
using Proxirae.Application.Services.Preferences;
using Proxirae.Application.Services.Preferences.Autostart;
using Proxirae.Application.Services.Routes;
using Proxirae.Contracts.DTOs.Flows;
using Proxirae.Contracts.DTOs.Logs;
using Proxirae.Contracts.DTOs.Routes;
using Proxirae.Presentation.WPF.Facades.Dialog;
using Proxirae.Presentation.WPF.Factories.Flow;
using Proxirae.Presentation.WPF.Factories.Routes;
using Proxirae.Presentation.WPF.ViewModels.About;
using Proxirae.Presentation.WPF.ViewModels.Flows;
using Proxirae.Presentation.WPF.ViewModels.Logs;
using Proxirae.Presentation.WPF.ViewModels.ProxyChecker;
using Proxirae.Presentation.WPF.ViewModels.ProxyRules;
using Proxirae.Presentation.WPF.ViewModels.ProxyServers;
using Proxirae.Presentation.WPF.ViewModels.Routes;
using System.Collections;
using System.Collections.ObjectModel;

namespace Proxirae.Presentation.WPF.ViewModels
{
    public partial class MainViewModel : ObservableObject, IDisposable
    {
        private readonly IApplicationService _applicationService;
        private readonly IClipboardService _clipboardService;
        private readonly DialogFacade _dialogFacade;
        private readonly IRouteViewModelFactory _routeViewModelFactory;
        private readonly IFlowViewModelFactory _flowViewModelFactory;
        private readonly IFlowService _flowService;
        private readonly ILogService _logService;
        private readonly IRouteService _routeService;
        private readonly IPreferencesService _preferencesService;
        private readonly IAutostartService _autostartService;

        [ObservableProperty]
        private int _selectedTabIndex;

        [ObservableProperty]
        private ObservableCollection<FlowViewModel> _flows = [];

        private readonly ObservableFixedSizeRingBuffer<LogViewModel> _logsBuffer = new(1000);
        public INotifyCollectionChangedSynchronizedViewList<LogViewModel> Logs { get; }

        private readonly ObservableFixedSizeRingBuffer<RouteViewModel> _routesBuffer = new(1000);
        public INotifyCollectionChangedSynchronizedViewList<RouteViewModel> Routes { get; }

        [ObservableProperty]
        private bool _routesAutoScroll = true;

        [ObservableProperty]
        private bool _logsAutoScroll = true;

        [ObservableProperty]
        private LogLevelDto _selectedLogLevel;

        [ObservableProperty]
        private bool _autostart;

        private CancellationTokenSource? _tabsHeightDebouceToken;

        [ObservableProperty]
        private double _tabsHeight;

        public MainViewModel(
            IApplicationService applicationService,
            IClipboardService clipboardService,
            DialogFacade dialogFacade,
            IRouteViewModelFactory routeViewModelFactory,
            IFlowViewModelFactory flowViewModelFactory,
            IFlowService flowService,
            ILogService logService,
            IRouteService routeService,
            IPreferencesService preferencesService,
            IAutostartService autostartService)
        {
            _applicationService = applicationService;
            _clipboardService = clipboardService;
            _dialogFacade = dialogFacade;
            _routeViewModelFactory = routeViewModelFactory;
            _flowViewModelFactory = flowViewModelFactory;
            _flowService = flowService;
            _logService = logService;
            _routeService = routeService;
            _preferencesService = preferencesService;
            _autostartService = autostartService;

            _flowService.FlowsUpdated += OnFlowsUpdated;
            _flowService.FlowClosed += OnFlowDeleted;

            Logs = _logsBuffer
                .CreateView(l => l)
                .ToNotifyCollectionChanged();

            _logService.LogReceived += OnLogReceived;

            Routes = _routesBuffer
                .CreateView(r => r)
                .ToNotifyCollectionChanged();

            _routeService.RouteReceived += OnRouteReceived;

            _selectedLogLevel = Enum.Parse<LogLevelDto>(_preferencesService.Current.Engine.LogLevel);
            _autostart = _preferencesService.Current.System.IsAutostartEnabled;
            _tabsHeight = _preferencesService.Current.Appearance.TabsHeight;
        }

        partial void OnTabsHeightChanged(double value)
        {
            _tabsHeightDebouceToken?.Cancel();
            _tabsHeightDebouceToken = new CancellationTokenSource();

            var token = _tabsHeightDebouceToken.Token;

            Task.Run(async () =>
            {
                try
                {
                    await Task.Delay(500, token);

                    var preferences = _preferencesService.Current.Appearance with { TabsHeight = value };
                    await _preferencesService.UpdateAsync(preferences, token);
                }
                catch (TaskCanceledException) { }
            }, token);
        }

        private async void OnRouteReceived(RouteDto route)
        {
            var routeViewModel = await _routeViewModelFactory.CreateAsync(route, CancellationToken.None);

            if (routeViewModel is null)
            {
                return;
            }

            WinApp.Current.Dispatcher.Invoke(() =>
            {
                _routesBuffer.AddLast(routeViewModel);
            });
        }

        private void OnLogReceived(LogDto log)
        {
            WinApp.Current.Dispatcher.Invoke(() =>
            {
                _logsBuffer.AddLast(new(log));
            });
        }

        private async void OnFlowsUpdated(IEnumerable<FlowDto> flows)
        {
            var activeIds = flows.Select(f => f.Id).ToHashSet();

            var existingIds = Flows.Select(x => x.Id).ToHashSet();
            var newFlowDtos = flows.Where(f => !existingIds.Contains(f.Id)).ToList();

            var newViewModels = new List<FlowViewModel>();

            foreach (var flow in newFlowDtos)
            {
                var vm = await _flowViewModelFactory.CreateAsync(flow, CancellationToken.None);
                if (vm != null) newViewModels.Add(vm);
            }

            WinApp.Current.Dispatcher.Invoke(() =>
            {
                var ghosts = Flows.Where(f => !activeIds.Contains(f.Id)).ToList();
                foreach (var ghost in ghosts)
                {
                    Flows.Remove(ghost);
                }

                foreach (var flow in flows)
                {
                    var existing = Flows.FirstOrDefault(x => x.Id == flow.Id);
                    existing?.Update(flow);
                }

                foreach (var vm in newViewModels)
                {
                    Flows.Add(vm);
                }
            });
        }

        private void OnFlowDeleted(FlowDto flow)
        {
            WinApp.Current.Dispatcher.Invoke(() =>
            {
                var target = Enumerable.FirstOrDefault(Flows, x => x.Id == flow.Id);
                if (target is not null)
                {
                    Flows.Remove(target);
                }
            });
        }

        [RelayCommand]
        private async Task UpdateAutostartAsync(CancellationToken cancellationToken)
        {
            var preferences = _preferencesService.Current.System with { IsAutostartEnabled = Autostart };

            _autostartService.SetAutostart(Autostart);
            await _preferencesService.UpdateAsync(preferences, cancellationToken);
        }

        [RelayCommand(CanExecute = nameof(CanDisconnect))]
        private async Task DisconnectAsync(FlowViewModel? flow, CancellationToken cancellationToken)
        {
            if (flow is null) return;

            await _flowService.DisconnectFlowAsync(flow.Id, cancellationToken);
        }

        private bool CanDisconnect(FlowViewModel? flow)
        {
            return flow is not null;
        }

        [RelayCommand(CanExecute = nameof(CanEnd))]
        private async Task EndAsync(FlowViewModel? flow, CancellationToken cancellationToken)
        {
            if (flow is null) return;

            await _flowService.EndFlowProcessAsync(flow.ProcessId, cancellationToken);
        }

        private bool CanEnd(FlowViewModel? flow)
        {
            return flow is not null;
        }

        [RelayCommand(CanExecute = nameof(CanCopy))]
        private void Copy(IEnumerable? selectedItems)
        {
            if (selectedItems == null) return;

            var lines = selectedItems
                .Cast<object>()
                .Where(item => item != null)
                .Select(item => item.ToString());

            var target = string.Join(Environment.NewLine, lines);

            if (!string.IsNullOrEmpty(target))
            {
                _clipboardService.Copy(target);
            }
        }

        private bool CanCopy(IEnumerable? selectedItems)
        {
            return selectedItems?.Cast<object>().Any() == true;
        }

        [RelayCommand]
        private void Clear(string? target = null)
        {
            var tabIndex = target switch
            {
                "Routes" => 0,
                "Logs" => 1,
                _ => SelectedTabIndex
            };

            if (tabIndex == 0)
            {
                _routesBuffer.Clear();
            }
            else if (tabIndex == 1)
            {
                _logsBuffer.Clear();
            }
        }

        [RelayCommand]
        private async Task ChangeLogLevelAsync(LogLevelDto? target, CancellationToken cancellationToken)
        {
            if (target == null) return;

            var preferences = _preferencesService.Current.Engine with { LogLevel = target.ToString()! };

            await _preferencesService.UpdateAsync(preferences, cancellationToken);
        }

        [RelayCommand]
        private void Restore()
        {
            _applicationService.Restore();
        }

        [RelayCommand]
        private void Exit()
        {
            _applicationService.Shutdown();
        }

        [RelayCommand]
        private void OpenProxyServers()
        {
            _dialogFacade.ShowDialog<ProxyServersViewModel>(this);
        }

        [RelayCommand]
        private void OpenProxyRules()
        {
            _dialogFacade.ShowDialog<ProxyRulesViewModel>(this);
        }

        [RelayCommand]
        private void OpenProxyChecker()
        {
            _dialogFacade.ShowDialog<ProxyCheckerViewModel>(this);
        }

        [RelayCommand]
        private async Task ResetLayoutAsync(CancellationToken cancellationToken)
        {
            var defaultAppearance = new AppearancePreferences();

            var updatedAppearance = _preferencesService.Current.Appearance with
            {
                TabsHeight = defaultAppearance.TabsHeight
            };

            await _preferencesService.UpdateAsync(updatedAppearance, cancellationToken);

            TabsHeight = updatedAppearance.TabsHeight;
        }

        [RelayCommand]
        private void OpenAbout()
        {
            _dialogFacade.ShowDialog<AboutViewModel>(this);
        }

        public void Dispose()
        {
            _flowService.FlowsUpdated -= OnFlowsUpdated;
            _flowService.FlowClosed -= OnFlowDeleted;
            _logService.LogReceived -= OnLogReceived;
        }
    }
}
