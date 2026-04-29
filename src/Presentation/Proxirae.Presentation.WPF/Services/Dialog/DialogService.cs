using System.Windows;

namespace Proxirae.Presentation.WPF.Services.Dialog
{
    internal class DialogService : IDialogService
    {
        private readonly ViewMappingRegistry _viewMappingRegistry = new();

        public DialogService(ViewMappingRegistry viewMappingRegistry)
        {
            _viewMappingRegistry = viewMappingRegistry;
        }

        private Window? GetOwner()
        {
            return WinApp.Current.Windows.OfType<Window>().FirstOrDefault(w => w.IsActive);
        }

        public void Show<TViewModel>() where TViewModel : class
        {
            _viewMappingRegistry.Map.TryGetValue(typeof(TViewModel), out var viewType);

            if (viewType == null)
            {
                throw new InvalidOperationException($"No view mapping found for {typeof(TViewModel).FullName}");
            }

            var viewModel = Activator.CreateInstance<TViewModel>();

            if (Activator.CreateInstance(viewType) is not Window view)
            {
                throw new InvalidOperationException($"The view mapped to {typeof(TViewModel).FullName} is not a Window.");
            }

            view.DataContext = viewModel;

            var owner = GetOwner();
            if (owner != null)
            {
                view.Owner = owner;
            }

            view.ShowDialog();
        }
    }
}
