using CommunityToolkit.Mvvm.ComponentModel;
using Proxirae.Contracts.DTOs.Flows;
using Proxirae.Presentation.WPF.Models.Endpoint;
using Proxirae.Presentation.WPF.Models.Process;
using System.Windows.Media;

namespace Proxirae.Presentation.WPF.ViewModels.Flows
{
    public partial class FlowViewModel : ObservableObject
    {
        public Guid Id { get; init; }
        public long ProcessId { get; init; }

        [ObservableProperty]
        private Endpoint _target = null!;

        [ObservableProperty]
        private string _processName;

        [ObservableProperty]
        private ImageSource? _processIcon;

        [ObservableProperty]
        private ulong _secondsPassed;

        [ObservableProperty]
        private ulong _bytesSent;

        [ObservableProperty]
        private ulong _bytesReceived;

        [ObservableProperty]
        private FlowStatusDto _status;

        [ObservableProperty]
        private string _actionName;

        public FlowViewModel(FlowDto dto, ProcessInfo processInfo, string actionName)
        {
            Id = dto.Id;
            ProcessId = dto.ProcessId;
            ProcessName = processInfo.Name;
            ProcessIcon = processInfo.Icon;
            ActionName = actionName;

            Update(dto);
        }

        public void Update(FlowDto dto)
        {
            Target = new Endpoint(dto.TargetAddress, dto.TargetPort);
            SecondsPassed = dto.SecondsPassed;
            BytesSent = dto.BytesSent;
            BytesReceived = dto.BytesReceived;
            Status = dto.Status;
        }
    }
}
