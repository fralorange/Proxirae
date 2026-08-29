#pragma warning disable CA1416

using Microsoft.Win32;
using System.Diagnostics;
using System.Reflection;

namespace Proxirae.Application.Services.Preferences.Autostart
{
    public class AutostartService : IAutostartService
    {
        private const string RegistryKeyPath = @"SOFTWARE\Microsoft\Windows\CurrentVersion\Run";
        private const string AppName = "Proxirae";

        public void SetAutostart(bool enable)
        {
#if DEBUG
            return;
#endif

            using RegistryKey? key = Registry.CurrentUser.OpenSubKey(RegistryKeyPath, true);
            if (key == null) return;

            if (enable)
            {
                var exePath = System.Environment.ProcessPath;

                if (string.IsNullOrEmpty(exePath)) return;

                key.SetValue(AppName, $"\"{exePath}\" --autostart");
            } else
            {
                key.DeleteValue(AppName, false);
            }

        }
    }
}

#pragma warning restore CA1416