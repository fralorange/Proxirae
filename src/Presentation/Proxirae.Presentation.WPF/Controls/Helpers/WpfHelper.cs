using System.Windows;

namespace Proxirae.Presentation.WPF.Controls.Helpers
{
    /// <summary>
    /// A class that helps with wpf controls.
    /// </summary>
    internal class WpfHelper
    {
        /// <summary>
        /// Gets element bounds relative to window.
        /// </summary>
        /// <param name="element"></param>
        /// <param name="w"></param>
        /// <returns></returns>
        public static Rect GetElementBoundsRelativeToWindow(FrameworkElement element, Window w)
        {
            return element.TransformToAncestor(w).TransformBounds(new Rect(element.RenderSize));
        }
    }
}
