using Proxirae.Contracts.DTOs.Routes;
using Proxirae.Presentation.WPF.Models.Endpoint;

namespace Proxirae.Presentation.WPF.ViewModels.Routes
{
    public class RouteViewModel
    {
        public DateTime Timestamp { get; set; }
        public long ProcessId { get; set; }
        public Endpoint Endpoint { get; set; }
        public string ActionName { get; set; } = null!;

        public RouteViewModel(RouteDto routeDto, string actionName)
        {
            Timestamp = routeDto.Timestamp;
            ProcessId = routeDto.ProcessId;
            Endpoint = new(routeDto.Address, routeDto.Port);
            ActionName = actionName;
        }

        public override string ToString()
        {
            return $"{Timestamp:HH:mm:ss} {ProcessId} - {Endpoint} - {ActionName}";
        }
    }
}
