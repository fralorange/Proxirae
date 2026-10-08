using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using MvvmDialogs.FrameworkDialogs.OpenFile;
using MvvmDialogs.FrameworkDialogs.SaveFile;
using ObservableCollections;
using Proxirae.Application.Exporters.Csv;
using Proxirae.Application.Models.Preferences.Appearance;
using Proxirae.Application.Models.Preferences.Metrics;
using Proxirae.Application.Services.Application;
using Proxirae.Application.Services.Browser;
using Proxirae.Application.Services.Clipboard;
using Proxirae.Application.Services.Flows;
using Proxirae.Application.Services.Logs;
using Proxirae.Application.Services.Package;
using Proxirae.Application.Services.Preferences;
using Proxirae.Application.Services.Routes;
using Proxirae.Contracts.DTOs.Flows;
using Proxirae.Contracts.DTOs.Logs;
using Proxirae.Contracts.DTOs.Routes;
using Proxirae.Presentation.WPF.Extensions;
using Proxirae.Presentation.WPF.Factories.Flow;
using Proxirae.Presentation.WPF.Factories.Routes;
using Proxirae.Presentation.WPF.Services.Dialog.File;
using Proxirae.Presentation.WPF.Services.Dialog.Input;
using Proxirae.Presentation.WPF.Services.Dialog.Message;
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
using System.Collections.Concurrent;
using System.Collections.ObjectModel;
using System.Threading.Channels;
using AppPreferences = Proxirae.Application.Models.Preferences.Preferences;

namespace Proxirae.Presentation.WPF.ViewModels
{
    public partial class MainViewModel : ObservableObject, IDisposable
    {
        private readonly IApplicationService _applicationService;
        private readonly IClipboardService _clipboardService;
        private readonly IFileDialogService _fileDialogService;
        private readonly IModalDialogService _modalDialogService;
        private readonly IMessageDialogService _messageDialogService;
        private readonly IInputDialogService _inputDialogService;
        private readonly IRouteViewModelFactory _routeViewModelFactory;
        private readonly IFlowViewModelFactory _flowViewModelFactory;
        private readonly IFlowService _flowService;
        private readonly ILogService _logService;
        private readonly IRouteService _routeService;
        private readonly IPreferencesService _preferencesService;
        private readonly IPackageService _packageService;
        private readonly ICsvExporter _csvExporter;
        private readonly IBrowserService _browserService;

        [ObservableProperty]
        private int _selectedTabIndex;

        [ObservableProperty]
        private ObservableCollection<FlowViewModel> _flows = [];

        private readonly ConcurrentDictionary<Guid, FlowViewModel> _flowsById = new();

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

        private CancellationTokenSource? _tabsHeightDebouceToken;

        private readonly Channel<LogDto> _logChannel =
            Channel.CreateUnbounded<LogDto>(new UnboundedChannelOptions { SingleReader = true });

        private readonly Channel<RouteDto> _routeChannel =
            Channel.CreateUnbounded<RouteDto>(new UnboundedChannelOptions { SingleReader = true });

        private readonly CancellationTokenSource _pumpCts = new();

        [ObservableProperty]
        private double _tabsHeight;

        public MainViewModel(
            IApplicationService applicationService,
            IClipboardService clipboardService,
            IFileDialogService fileDialogService,
            IModalDialogService modalDialogService,
            IMessageDialogService messageDialogService,
            IInputDialogService inputDialogService,
            IRouteViewModelFactory routeViewModelFactory,
            IFlowViewModelFactory flowViewModelFactory,
            IFlowService flowService,
            ILogService logService,
            IRouteService routeService,
            IPreferencesService preferencesService,
            IPackageService packageService,
            ICsvExporter csvExporter,
            IBrowserService browserService)
        {
            _applicationService = applicationService;
            _clipboardService = clipboardService;
            _fileDialogService = fileDialogService;
            _modalDialogService = modalDialogService;
            _messageDialogService = messageDialogService;
            _inputDialogService = inputDialogService;
            _routeViewModelFactory = routeViewModelFactory;
            _flowViewModelFactory = flowViewModelFactory;
            _flowService = flowService;
            _logService = logService;
            _routeService = routeService;
            _preferencesService = preferencesService;
            _packageService = packageService;
            _csvExporter = csvExporter;
            _browserService = browserService;

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

            _ = PumpLogsAsync(_pumpCts.Token);
            _ = PumpRoutesAsync(_pumpCts.Token);

            _selectedLogLevel = Enum.Parse<LogLevelDto>(_preferencesService.Current.Engine.LogLevel);
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

        private void OnRouteReceived(RouteDto route)
        {
            _routeChannel.Writer.TryWrite(route);
        }

        private void OnLogReceived(LogDto log)
        {
            _logChannel.Writer.TryWrite(log);
        }

        private async Task PumpRoutesAsync(CancellationToken cancellationToken)
        {
            var batch = new List<RouteDto>(256);

            try
            {
                while (await _routeChannel.Reader.WaitToReadAsync(cancellationToken).ConfigureAwait(false))
                {
                    while (_routeChannel.Reader.TryRead(out var route))
                    {
                        batch.Add(route);
                    }

                    await Task.Delay(TimeSpan.FromMilliseconds(50), cancellationToken).ConfigureAwait(false);

                    if (batch.Count == 0)
                    {
                        continue;
                    }

                    var swap = batch;
                    batch = new List<RouteDto>(256);

                    var viewModels = new List<RouteViewModel>(swap.Count);

                    foreach (var route in swap)
                    {
                        var viewModel = await _routeViewModelFactory.CreateAsync(route, cancellationToken).ConfigureAwait(false);
                        if (viewModel is not null)
                        {
                            viewModels.Add(viewModel);
                        }
                    }

                    await WinApp.Current.Dispatcher.InvokeAsync(() =>
                    {
                        foreach (var viewModel in viewModels)
                        {
                            _routesBuffer.AddLast(viewModel);
                        }

                        ExportRoutesHistoryCommand.NotifyCanExecuteChanged();
                    });
                }
            }
            catch (OperationCanceledException)
            {
            }
        }

        private async Task PumpLogsAsync(CancellationToken cancellationToken)
        {
            var batch = new List<LogViewModel>(256);

            try
            {
                while (await _logChannel.Reader.WaitToReadAsync(cancellationToken).ConfigureAwait(false))
                {
                    while (_logChannel.Reader.TryRead(out var log))
                    {
                        batch.Add(new LogViewModel(log));
                    }

                    await Task.Delay(TimeSpan.FromMilliseconds(50), cancellationToken).ConfigureAwait(false);

                    if (batch.Count == 0)
                    {
                        continue;
                    }

                    var swap = batch;
                    batch = new List<LogViewModel>(256);

                    await WinApp.Current.Dispatcher.InvokeAsync(() =>
                    {
                        foreach (var viewModel in swap)
                        {
                            _logsBuffer.AddLast(viewModel);
                        }

                        ExportLogsHistoryCommand.NotifyCanExecuteChanged();
                    });
                }
            }
            catch (OperationCanceledException)
            {
            }
        }

        private async void OnFlowsUpdated(IEnumerable<FlowDto> flows)
        {
            var snapshot = flows.ToList();
            var activeIds = snapshot.Select(f => f.Id).ToHashSet();

            var toCreate = new List<FlowDto>();

            foreach (var flow in snapshot)
            {
                if (!_flowsById.ContainsKey(flow.Id))
                {
                    toCreate.Add(flow);
                }
            }

            var newViewModels = new List<FlowViewModel>(toCreate.Count);

            foreach (var flow in toCreate)
            {
                var vm = await _flowViewModelFactory.CreateAsync(flow, CancellationToken.None);
                if (vm is not null)
                {
                    newViewModels.Add(vm);
                }
            }

            await WinApp.Current.Dispatcher.InvokeAsync(() =>
            {
                foreach (var pair in _flowsById)
                {
                    if (!activeIds.Contains(pair.Key) && _flowsById.TryRemove(pair.Key, out var ghost))
                    {
                        Flows.Remove(ghost);
                    }
                }

                foreach (var flow in snapshot)
                {
                    if (_flowsById.TryGetValue(flow.Id, out var existing))
                    {
                        existing.Update(flow);
                    }
                }

                foreach (var vm in newViewModels)
                {
                    _flowsById[vm.Id] = vm;
                    Flows.Add(vm);
                }
            });
        }

        private void OnFlowDeleted(FlowDto flow)
        {
            WinApp.Current.Dispatcher.Invoke(() =>
            {
                if (_flowsById.TryRemove(flow.Id, out var target))
                {
                    Flows.Remove(target);
                }
            });
        }

        [RelayCommand]
        private async Task ImportConfigurationAsync(CancellationToken cancellationToken)
        {
            var settings = new OpenFileDialogSettings
            {
                Title = "Import Configuration",
                InitialDirectory = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments),
                Filter = "Proxirae Configuration (*.pxcfg)|*.pxcfg"
            };

            var path = _fileDialogService.ShowOpenFileDialog(this, settings);
            if (path is null) return;

            var success = await _packageService.ImportAsync(path, ct =>
            {
                var (masterPassword, result) = _inputDialogService.ShowText(
                    this,
                    "ConfigMasterPasswordImport",
                    "ConfigMasterPasswordImportTitle",
                    "ConfigMasterPasswordImportAlt");

                if (result == InputDialogResult.Cancel || result == InputDialogResult.None)
                {
                    return Task.FromResult<string?>(null);
                }

                return Task.FromResult<string?>(masterPassword);
            }, cancellationToken);

            if (!success)
            {
                _messageDialogService.ShowError(this, "ConfigCorruptedOrIncorrent", "ConfigCorruptedOrIncorrentTitle");
            }
        }

        [RelayCommand]
        private async Task ExportConfigurationAsync(CancellationToken cancellationToken)
        {
            var settings = new SaveFileDialogSettings
            {
                Title = "Export Configuration",
                InitialDirectory = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments),
                Filter = "Proxirae Configuration (*.pxcfg)|*.pxcfg",
                DefaultExt = ".pxcfg",
                AddExtension = true,
                OverwritePrompt = true,
                FileName = "config.pxcfg"
            };

            var path = _fileDialogService.ShowSaveFileDialog(this, settings);
            if (path is null) return;

            var success = await _packageService.ExportAsync(path, ct =>
            {
                var (masterPassword, result) = _inputDialogService.ShowText(
                    this,
                    "ConfigMasterPasswordExport",
                    "ConfigMasterPasswordExportTitle");

                if (result != InputDialogResult.Ok)
                {
                    return Task.FromResult<string?>(null);
                }

                return Task.FromResult<string?>(masterPassword);
            }, cancellationToken);

            if (success == false)
            {
                _messageDialogService.ShowError(this, "ConfigDoesNotExist", "ConfigDoesNotExistTitle");
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

            var path = _fileDialogService.ShowSaveFileDialog(this, settings);

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

            var path = _fileDialogService.ShowSaveFileDialog(this, settings);

            if (path is not null)
            {
                await _csvExporter.ExportAsync(path, _logsBuffer, cancellationToken);
            }
        }

        private bool CanExportLogs()
        {
            return _logsBuffer.Count > 0;
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
            if (flow is null || !_messageDialogService.ShowWarning(this, "EndProcess", "EndProcessTitle"))
            {
                return;
            }

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
            _routeService.RouteReceived -= OnRouteReceived;

            _pumpCts.Cancel();
            _pumpCts.Dispose();
        }
    }
}
