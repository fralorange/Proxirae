using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using ObservableCollections;
using Proxirae.Application.Facades.Actions;
using Proxirae.Application.Facades.Routes;
using Proxirae.Application.Services.Application;
using Proxirae.Application.Services.Clipboard;
using Proxirae.Application.Services.Flows;
using Proxirae.Application.Services.Logs;
using Proxirae.Application.Services.Preferences;
using Proxirae.Application.Services.Routes;
using Proxirae.Contracts.DTOs.Flows;
using Proxirae.Contracts.DTOs.Logs;
using Proxirae.Contracts.DTOs.Routes;
using Proxirae.Presentation.WPF.Facades.Dialog;
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
        private readonly ActionFacade _actionFacade;
        private readonly RouteFacade _routeFacade;
        private readonly IFlowService _flowService;
        private readonly ILogService _logService;
        private readonly IRouteService _routeService;
        private readonly IPreferencesService _preferencesService;

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

        public MainViewModel(
            IApplicationService applicationService,
            IClipboardService clipboardService,
            DialogFacade dialogFacade,
            ActionFacade actionFacade,
            RouteFacade routeFacade,
            IFlowService flowService,
            ILogService logService,
            IRouteService routeService,
            IPreferencesService preferencesService)
        {
            _applicationService = applicationService;
            _clipboardService = clipboardService;
            _dialogFacade = dialogFacade;
            _actionFacade = actionFacade;
            _routeFacade = routeFacade;
            _flowService = flowService;
            _logService = logService;
            _routeService = routeService;
            _preferencesService = preferencesService;

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
        }

        private void OnRouteReceived(RouteDto route)
        {
            WinApp.Current.Dispatcher.Invoke(async () =>
            {
                var actionName = await _routeFacade.GetActionNameAsync(route.RuleId, CancellationToken.None);

                if (actionName == null)
                {
                    return;
                }

                var routeViewModel = new RouteViewModel(route, actionName);
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

        private void OnFlowsUpdated(IEnumerable<FlowDto> flows)
        {
            var activeIds = flows.Select(f => f.Id).ToHashSet();

            WinApp.Current.Dispatcher.InvokeAsync(async () =>
            {
                var ghosts = Flows.Where(f => !activeIds.Contains(f.Id)).ToList();
                foreach (var ghost in ghosts)
                {
                    Flows.Remove(ghost);
                }

                foreach (var flow in flows)
                {
                    var existing = Enumerable.FirstOrDefault(Flows, x => x.Id == flow.Id);
                    if (existing is not null)
                    {
                        existing.Update(flow);
                    }
                    else
                    {
                        var action = await _actionFacade.GetActionAsync(flow.ProxyId, CancellationToken.None);

                        if (!Flows.Any(x => x.Id == flow.Id))
                        {
                            Flows.Add(new FlowViewModel(flow, action.Name));
                        }
                    }
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

        public void Dispose()
        {
            _flowService.FlowsUpdated -= OnFlowsUpdated;
            _flowService.FlowClosed -= OnFlowDeleted;
            _logService.LogReceived -= OnLogReceived;
        }
    }
}
