using Microsoft.Win32;

namespace Proxirae.Presentation.WPF.Controls.Helpers
{
    /// <summary>
    /// A class helper that helps with native controls.
    /// </summary>
    internal class NativeHelper
    {
        // ───────────────────────────────────────
        // Window message constants (WinAPI)
        // ───────────────────────────────────────

        /// <summary>
        /// Posted when the cursor moves within a window and mouse input needs to be hit-tested (e.g., for resizing).
        /// </summary>
        public const int WM_NCHITTEST = 0x0084;

        /// <summary>
        /// Indicates the user pressed the left mouse button in a non-client area (like the title bar).
        /// </summary>
        public const int WM_NCLBUTTONDOWN = 0x00A1;

        /// <summary>
        /// Indicates the user released the left mouse button in a non-client area.
        /// </summary>
        public const int WM_NCLBUTTONUP = 0x00A2;

        /// <summary>
        /// Indicates the user double-clicked the left mouse button in a non-client area.
        /// </summary>
        public const int WM_NCLBUTTONDBLCLK = 0x00A3;

        /// <summary>
        /// Sent to a window before it is resized to determine its maximum and minimum size.
        /// </summary>
        public const int WM_GETMINMAXINFO = 0x0024;

        /// <summary>
        /// Sent when the user selects a command from the window menu or presses a system key.
        /// </summary>
        public const int WM_SYSCOMMAND = 0x0112;

        /// <summary>
        /// Hit-test result value indicating the max (maximize) button.
        /// </summary>
        public const int HTMAXBUTTON = 9;

        // ───────────────────────────────────────
        // Feature checks
        // ───────────────────────────────────────

        /// <summary>
        /// Determines whether the Windows Snap Layout feature is enabled.
        /// </summary>
        /// <returns>
        /// <c>true</c> if Snap Layouts are enabled (on supported systems); otherwise, <c>false</c>.
        /// </returns>
        public static bool IsSnapLayoutEnabled()
        {
            if (!IsWindows11())
            {
                return false;
            }

            using RegistryKey? key = Registry.CurrentUser.OpenSubKey(@"Software\Microsoft\Windows\CurrentVersion\Explorer\Advanced");
            object? registryValueObject = key?.GetValue("EnableSnapAssistFlyout");

            if (registryValueObject == null)
            {
                return true;
            }

            int registryValue = (int)registryValueObject;

            return registryValue > 0;
        }

        /// <summary>
        /// Checks whether the current OS version is Windows 11 or newer.
        /// </summary>
        /// <returns>
        /// <c>true</c> if the OS is Windows 11 (build 22000+) or newer; otherwise, <c>false</c>.
        /// </returns>
        public static bool IsWindows11()
        {
            return Environment.OSVersion.Version.Major == 10 &&
                Environment.OSVersion.Version.Minor == 0 &&
                Environment.OSVersion.Version.Build >= 22000;
        }
    }
}
