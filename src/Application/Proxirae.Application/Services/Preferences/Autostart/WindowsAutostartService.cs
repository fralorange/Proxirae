#pragma warning disable CA1416

using Microsoft.Win32;

namespace Proxirae.Application.Services.Preferences.Autostart
{
    public class WindowsAutostartService : IAutostartService
    {
        private const string RunKeyPath = @"SOFTWARE\Microsoft\Windows\CurrentVersion\Run";
        private const string ApprovedKeyPath = @"SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\StartupApproved\Run";
        private const string AppName = nameof(Proxirae);

        public void SetAutostart(bool enable)
        {
#if DEBUG
            return;
#endif
            EnsureRunKeyExists();

            using var approvedKey = Registry.CurrentUser.CreateSubKey(ApprovedKeyPath);
            if (approvedKey == null) return;

            if (enable)
            {
                approvedKey.DeleteValue(AppName, false);
            }
            else
            {
                byte[] disabledData = [0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00];
                approvedKey.SetValue(AppName, disabledData, RegistryValueKind.Binary);
            }
        }

        private void EnsureRunKeyExists()
        {
            using RegistryKey? runKey = Registry.CurrentUser.OpenSubKey(RunKeyPath, true);
            if (runKey == null) return;

            var exePath = System.Environment.ProcessPath;
            if (string.IsNullOrEmpty(exePath)) return;

            string expectedValue = $"\"{exePath}\" --autostart";
            string? currentValue = runKey.GetValue(AppName) as string;

            if (currentValue != expectedValue)
            {
                runKey.SetValue(AppName, expectedValue);
            }
        }
    }
}

#pragma warning restore CA1416