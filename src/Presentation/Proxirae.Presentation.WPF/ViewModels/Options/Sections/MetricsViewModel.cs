using CommunityToolkit.Mvvm.ComponentModel;
using Proxirae.Application.Models.Preferences;
using Proxirae.Application.Services.Preferences;
using System.ComponentModel.DataAnnotations;

namespace Proxirae.Presentation.WPF.ViewModels.Options.Sections
{
    public partial class MetricsViewModel : BaseSectionViewModel
    {
        [Range(10, 10000)]
        [ObservableProperty]
        [NotifyPropertyChangedFor(nameof(HasChanges))]
        private int _routingBufferSize = 1000;

        [Range(10, 10000)]
        [ObservableProperty]
        [NotifyPropertyChangedFor(nameof(HasChanges))]
        private int _logsBufferSize = 1000;

        public MetricsViewModel(IPreferencesService preferencesService)
        {
            RoutingBufferSize = preferencesService.Current.Metrics.RoutingBufferSize;
            LogsBufferSize = preferencesService.Current.Metrics.LogsBufferSize;

            HasChanges = false;
        }

        public override Preferences ApplyChanges(Preferences current)
        {
            return current with
            {
                Metrics = current.Metrics with
                {
                    RoutingBufferSize = RoutingBufferSize,
                    LogsBufferSize = LogsBufferSize,
                }
            };
        }

        partial void OnLogsBufferSizeChanged(int value)
        {
            HasChanges = true;
        }

        partial void OnRoutingBufferSizeChanged(int value)
        {
            HasChanges = true;
        }
    }
}
