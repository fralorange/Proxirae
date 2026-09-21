using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using MvvmDialogs.FrameworkDialogs.OpenFile;
using MvvmDialogs.FrameworkDialogs.SaveFile;
using ObservableCollections;
using Proxirae.Application.Exporters.Csv;
using Proxirae.Application.Facades.Configuration;
using Proxirae.Application.Models.Preferences.Appearance;
using Proxirae.Application.Models.Preferences.Metrics;
using Proxirae.Application.Services.Application;
using Proxirae.Application.Services.Archive;
using Proxirae.Application.Services.Browser;
using Proxirae.Application.Services.Clipboard;
using Proxirae.Application.Services.Flows;
using Proxirae.Application.Services.Logs;
using Proxirae.Application.Services.Preferences;
using Proxirae.Application.Services.Preferences.Autostart;
using Proxirae.Application.Services.Routes;
using Proxirae.Contracts.DTOs.Flows;
using Proxirae.Contracts.DTOs.Logs;
using Proxirae.Contracts.DTOs.Routes;
using Proxirae.Presentation.WPF.Extensions;
using Proxirae.Presentation.WPF.Factories.Flow;
using Proxirae.Presentation.WPF.Factories.Routes;
using Proxirae.Presentation.WPF.Services.Dialog.File;
using Proxirae.Presentation.WPF.Services.Dialog.Modal;
using Proxirae.Presentation.WPF.ViewModels.About;
using Proxirae.Presentation.WPF.ViewModels.Flows;
using Proxirae.Presentation.WPF.ViewModels.Logs;
using Proxirae.Presentation.WPF.ViewModels.Options;
using Proxirae.Presentation.WPF.ViewModels.ProxyChecker;
using Proxirae.Presentation.WPF.ViewModels.ProxyRules;
using Proxirae.Presentation.WPF.ViewModels.ProxyServers;
using Proxirae.Presentation.WPF.ViewModels.Routes;
using System.Collections;
using System.Collections.ObjectModel;
using AppPreferences = Proxirae.Application.Models.Preferences.Preferences;

namespace Proxirae.Presentation.WPF.ViewModels
{
    public partial class MainViewModel : ObservableObject, IDisposable
    {
        private readonly IApplicationService _applicationService;
        private readonly IClipboardService _clipboardService;
        private readonly IFileDialogService _fileDialogService;
        private readonly IModalDialogService _modalDialogService;
        private readonly IConfigurationFacade _configurationFacade;
        private readonly IRouteViewModelFactory _routeViewModelFactory;
        private readonly IFlowViewModelFactory _flowViewModelFactory;
        private readonly IFlowService _flowService;
        private readonly ILogService _logService;
        private readonly IRouteService _routeService;
        private readonly IPreferencesService _preferencesService;
        private readonly IAutostartService _autostartService;
        private readonly IArchiveService _archiveService;
        private readonly ICsvExporter _csvExporter;
        private readonly IBrowserService _browserService;

        [ObservableProperty]
        private int _selectedTabIndex;

        [ObservableProperty]
        private ObservableCollection<FlowViewModel> _flows = [];

        private ObservableFixedSizeRingBuffer<LogViewModel> _logsBuffer;

        [ObservableProperty]
        private INotifyCollectionChangedSynchronizedViewList<LogViewModel> _logs;

        private ObservableFixedSizeRingBuffer<RouteViewModel> _routesBuffer;

        [ObservableProperty]
        private INotifyCollectionChangedSynchronizedViewList<RouteViewModel> _routes;

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
            IConfigurationFacade configurationFacade,
            IRouteViewModelFactory routeViewModelFactory,
            IFlowViewModelFactory flowViewModelFactory,
            IFlowService flowService,
            ILogService logService,
            IRouteService routeService,
            IPreferencesService preferencesService,
            IAutostartService autostartService,
            IArchiveService archiveService,
            ICsvExporter csvExporter,
            IBrowserService browserService,
            IFileDialogService fileDialogService,
            IModalDialogService modalDialogService)
        {
            _applicationService = applicationService;
            _clipboardService = clipboardService;
            _configurationFacade = configurationFacade;
            _routeViewModelFactory = routeViewModelFactory;
            _flowViewModelFactory = flowViewModelFactory;
            _flowService = flowService;
            _logService = logService;
            _routeService = routeService;
            _preferencesService = preferencesService;
            _autostartService = autostartService;
            _archiveService = archiveService;
            _csvExporter = csvExporter;
            _browserService = browserService;
            _fileDialogService = fileDialogService;
            _modalDialogService = modalDialogService;

            _flowService.FlowsUpdated += OnFlowsUpdated;
            _flowService.FlowClosed += OnFlowDeleted;

            _preferencesService.PreferencesChanged += OnPreferencesChanged;

            _logsBuffer = new(_preferencesService.Current.Metrics.LogsBufferSize);

            _logs = _logsBuffer
                .CreateView(l => l)
                .ToNotifyCollectionChanged();

            _logService.LogReceived += OnLogReceived;

            _routesBuffer = new(_preferencesService.Current.Metrics.RoutingBufferSize);

            _routes = _routesBuffer
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

        private void OnPreferencesChanged(object? sender, AppPreferences newPreferences)
        {
            WinApp.Current.Dispatcher.Invoke(() =>
            {
                SyncMetricsPreferences(newPreferences.Metrics);
            });
        }

        private void SyncMetricsPreferences(MetricsPreferences metrics)
        {
            if (_logsBuffer.Capacity != metrics.LogsBufferSize)
            {
                (_logsBuffer, Logs) = _logsBuffer.Resize(Logs, metrics.LogsBufferSize);
            }

            if (_routesBuffer.Capacity != metrics.RoutingBufferSize)
            {
                (_routesBuffer, Routes) = _routesBuffer.Resize(Routes, metrics.RoutingBufferSize);
            }
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

                ExportRoutesHistoryCommand.NotifyCanExecuteChanged();
            });
        }

        private void OnLogReceived(LogDto log)
        {
            WinApp.Current.Dispatcher.Invoke(() =>
            {
                _logsBuffer.AddLast(new(log));

                ExportLogsHistoryCommand.NotifyCanExecuteChanged();
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
        private async Task ImportConfiguration(CancellationToken cancellationToken)
        {
            var settings = new OpenFileDialogSettings
            {
                Title = "Import Configuration",
                InitialDirectory = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments),
                Filter = "Proxirae Configuration (*.pxcfg)|*.pxcfg"
            };

            var path = _fileDialogService.OpenFile(this, settings);
            if (path is null) return;

            var success = _archiveService.ExtractArchive(path, _configurationFacade.AppDataDirectory);

            if (success)
            {
                await _configurationFacade.ReloadAsync(cancellationToken);
            }
        }

        [RelayCommand]
        private void ExportConfiguration()
        {
            var settings = new SaveFileDialogSettings
            {
                Title = "Export Configuration",
                InitialDirectory = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments),
                Filter = "Proxirae Configuration (*.pxcfg)|*.pxcfg",
                DefaultExt = ".pxcfg",
                AddExtension = true,
                FileName = "config.pxcfg"
            };

            var path = _fileDialogService.SaveFile(this, settings);

            if (path is not null)
            {
                _archiveService.CreateArchive(path, _configurationFacade.ConfigurationFiles);
            }
        }

        [RelayCommand(CanExecute = nameof(CanExportRoutes))]
        private async Task ExportRoutesHistoryAsync(CancellationToken cancellationToken)
        {
            if (_routesBuffer.Count == 0) return;

            var settings = new SaveFileDialogSettings
            {
                Title = "Export Routing History",
                InitialDirectory = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments),
                Filter = "CSV File (*.csv)|*.csv",
                DefaultExt = "csv",
                AddExtension = true,
                FileName = $"routing_history_{DateTime.Now:yyyyMMdd_HHmmss}.csv"
            };

            var path = _fileDialogService.SaveFile(this, settings);

            if (path is not null)
            {
                await _csvExporter.ExportAsync(path, _routesBuffer, cancellationToken);
            }
        }

        private bool CanExportRoutes()
        {
            return _routesBuffer.Count > 0;
        }

        [RelayCommand(CanExecute = nameof(CanExportLogs))]
        private async Task ExportLogsHistoryAsync(CancellationToken cancellationToken)
        {
            if (_logsBuffer.Count == 0) return;

            var settings = new SaveFileDialogSettings
            {
                Title = "Export Logs History",
                InitialDirectory = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments),
                Filter = "CSV File (*.csv)|*.csv",
                DefaultExt = "csv",
                AddExtension = true,
                FileName = $"logs_history_{DateTime.Now:yyyyMMdd_HHmmss}.csv"
            };

            var path = _fileDialogService.SaveFile(this, settings);

            if (path is not null)
            {
                await _csvExporter.ExportAsync(path, _logsBuffer, cancellationToken);
            }
        }

        private bool CanExportLogs()
        {
            return _logsBuffer.Count > 0;
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
                ExportRoutesHistoryCommand.NotifyCanExecuteChanged();
            }
            else if (tabIndex == 1)
            {
                _logsBuffer.Clear();
                ExportLogsHistoryCommand.NotifyCanExecuteChanged();
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
            _modalDialogService.ShowDialog<ProxyServersViewModel>(this);
        }

        [RelayCommand]
        private void OpenProxyRules()
        {
            _modalDialogService.ShowDialog<ProxyRulesViewModel>(this);
        }

        [RelayCommand]
        private void OpenProxyChecker()
        {
            _modalDialogService.ShowDialog<ProxyCheckerViewModel>(this);
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
        private void OpenOptions()
        {
            _modalDialogService.ShowDialog<OptionsViewModel>(this);
        }

        [RelayCommand]
        private void CheckForUpdates()
        {
            _browserService.OpenUrl("https://github.com/fralorange/Proxirae/releases"); // TODO: Integrate with GitHub API in future
        }

        [RelayCommand]
        private void OpenAbout()
        {
            _modalDialogService.ShowDialog<AboutViewModel>(this);
        }

        public void Dispose()
        {
            _preferencesService.PreferencesChanged -= OnPreferencesChanged;
            _flowService.FlowsUpdated -= OnFlowsUpdated;
            _flowService.FlowClosed -= OnFlowDeleted;
            _logService.LogReceived -= OnLogReceived;
        }
    }
}
