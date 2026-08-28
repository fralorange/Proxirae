using CommunityToolkit.Mvvm.ComponentModel;
using Proxirae.Contracts.DTOs.Flows;
using Proxirae.Presentation.WPF.Models.Endpoint;

namespace Proxirae.Presentation.WPF.ViewModels.Flows
{
    public partial class FlowViewModel : ObservableObject
    {
        public Guid Id { get; set; }

        [ObservableProperty]
        private Endpoint _target = null!;

        [ObservableProperty]
        private long _processId;

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

        public FlowViewModel(FlowDto dto, string actionName)
        {
            Id = dto.Id;
            ActionName = actionName;

            Update(dto);
        }

        public void Update(FlowDto dto)
        {
            Target = new Endpoint(dto.TargetAddress, dto.TargetPort);
            ProcessId = dto.ProcessId;
            SecondsPassed = dto.SecondsPassed;
            BytesSent = dto.BytesSent;
            BytesReceived = dto.BytesReceived;
            Status = dto.Status;
        }
    }
}
