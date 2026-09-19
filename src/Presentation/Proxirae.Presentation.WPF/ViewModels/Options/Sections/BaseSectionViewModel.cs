using CommunityToolkit.Mvvm.ComponentModel;
using Proxirae.Application.Models.Preferences;

namespace Proxirae.Presentation.WPF.ViewModels.Options.Sections
{
    public abstract class BaseSectionViewModel : ObservableValidator, ISectionViewModel
    {
        private bool _hasChanges;
        public bool HasChanges
        {
            get => _hasChanges;
            set => SetProperty(ref _hasChanges, value);
        }

        public abstract Preferences ApplyChanges(Preferences current);

        public bool Validate()
        {
            ValidateAllProperties();

            return !HasErrors;
        }
    }
}
