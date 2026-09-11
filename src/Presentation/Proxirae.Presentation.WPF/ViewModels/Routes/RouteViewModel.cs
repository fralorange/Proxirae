using Proxirae.Contracts.DTOs.Routes;
using Proxirae.Presentation.WPF.Models.Endpoint;

namespace Proxirae.Presentation.WPF.ViewModels.Routes
{
    public class RouteViewModel
    {
        public DateTime Timestamp { get; init; }
        public long ProcessId { get; init; }
        public string ProcessName { get; init; }
        public Endpoint Endpoint { get; init; }
        public string ActionName { get; init; }

        public RouteViewModel(RouteDto routeDto, string processName, string actionName)
        {
            Timestamp = routeDto.Timestamp;
            ProcessId = routeDto.ProcessId;
            Endpoint = new(routeDto.Address, routeDto.Port);
            ProcessName = processName;
            ActionName = actionName;
        }
    }
}
