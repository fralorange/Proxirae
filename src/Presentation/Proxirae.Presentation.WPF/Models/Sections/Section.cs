using System.ComponentModel;

namespace Proxirae.Presentation.WPF.Models.Sections
{
    public class Section
    {
        public required string Title { get; set; }
        public required INotifyPropertyChanged ViewModel { get; set; }
    }
}
