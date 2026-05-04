using MvvmDialogs.DialogTypeLocators;
using System.ComponentModel;

namespace Proxirae.Presentation.WPF
{
    internal class TypeLocator : IDialogTypeLocator
    {
        public Type Locate(INotifyPropertyChanged viewModel)
        {
            Type viewModelType = viewModel.GetType();
            var viewModelNamespace = viewModelType.Namespace;

            if (viewModelNamespace?.EndsWith("ViewModels") == true)
            {
                var viewNamespace = string.Concat(viewModelNamespace.AsSpan(0, viewModelNamespace.Length - ".ViewModels".Length), ".Views");
                var viewName = viewModelType.Name.EndsWith("ViewModel") ? viewModelType.Name.Substring(0, viewModelType.Name.Length - "Model".Length) : viewModelType.Name;
                var fullViewName = $"{viewNamespace}.{viewName}";
                var viewType = Type.GetType(fullViewName);
                if (viewType != null)
                {
                    return viewType;
                }
            }

            throw new InvalidOperationException($"Cannot locate view for view model type {viewModelType.FullName}");
        }
    }
}
