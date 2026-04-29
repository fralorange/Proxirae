using Proxirae.Presentation.WPF.ViewModels;
using Proxirae.Presentation.WPF.Views;

namespace Proxirae.Presentation.WPF
{
    internal class ViewMappingRegistry
    {
        public Dictionary<Type, Type> Map { get; } = new()
        {
            { typeof(ProxyServersViewModel), typeof(ProxyServersView) },
        };
    }
}
