using Microsoft.Win32;
using Microsoft.Win32.TaskScheduler;
using Proxirae.Application.Services.Preferences.Autostart;
using System.IO;
using System.Security.Principal;

namespace Proxirae.Presentation.WPF.Services.Preferences.Autostart
{
    public sealed class WindowsAutostartService : IAutostartService
    {
        private const string TaskName = "Proxirae Autostart";

        public void SetAutostart(
            bool enable,
            bool silentStart,
            bool startMinimized)
        {
#if DEBUG
            return;
#endif

            RemoveLegacyRegistryAutostart();

            if (!enable)
            {
                DeleteAutostartTask();
                return;
            }

            if (silentStart && startMinimized)
            {
                throw new InvalidOperationException(
                    "Silent start and minimized start cannot be enabled simultaneously.");
            }

            CreateAutostartTask(
                silentStart,
                startMinimized);
        }

        private static void CreateAutostartTask(
            bool silentStart,
            bool startMinimized)
        {
            string exePath = Environment.ProcessPath
                ?? throw new InvalidOperationException(
                    "Unable to determine application executable path.");

            string workingDirectory = Path.GetDirectoryName(exePath)
                ?? throw new InvalidOperationException(
                    "Unable to determine application working directory.");

            using var taskService = new TaskService();

            TaskDefinition taskDefinition = taskService.NewTask();

            taskDefinition.RegistrationInfo.Description =
                "Starts Proxirae automatically when the user logs in.";

            taskDefinition.RegistrationInfo.Author = "Proxirae";

            string userName = WindowsIdentity.GetCurrent().Name;

            taskDefinition.Principal.UserId = userName;
            taskDefinition.Principal.LogonType =
                TaskLogonType.InteractiveToken;
            taskDefinition.Principal.RunLevel =
                TaskRunLevel.Highest;

            var logonTrigger = new LogonTrigger
            {
                UserId = userName
            };

            taskDefinition.Triggers.Add(logonTrigger);

            string arguments = BuildArguments(
                silentStart,
                startMinimized);

            var action = new ExecAction(
                exePath,
                arguments,
                workingDirectory);

            taskDefinition.Actions.Add(action);

            taskDefinition.Settings.Enabled = true;
            taskDefinition.Settings.Hidden = false;
            taskDefinition.Settings.StartWhenAvailable = true;
            taskDefinition.Settings.DisallowStartIfOnBatteries = false;
            taskDefinition.Settings.StopIfGoingOnBatteries = false;
            taskDefinition.Settings.MultipleInstances =
                TaskInstancesPolicy.IgnoreNew;

            taskService.RootFolder.RegisterTaskDefinition(
                TaskName,
                taskDefinition,
                TaskCreation.CreateOrUpdate,
                userName,
                null,
                TaskLogonType.InteractiveToken,
                null);
        }

        private static string BuildArguments(
            bool silentStart,
            bool startMinimized)
        {
            if (silentStart && startMinimized)
            {
                throw new InvalidOperationException(
                    "Silent start and minimized start cannot be enabled simultaneously.");
            }

            if (silentStart)
            {
                return "--autostart --silent";
            }

            if (startMinimized)
            {
                return "--autostart --minimized";
            }

            return "--autostart";
        }

        private static void DeleteAutostartTask()
        {
            using var taskService = new TaskService();

            taskService.RootFolder.DeleteTask(
                TaskName,
                exceptionOnNotExists: false);
        }

        private static void RemoveLegacyRegistryAutostart()
        {
            const string runKeyPath =
                @"Software\Microsoft\Windows\CurrentVersion\Run";

            const string approvedKeyPath =
                @"Software\Microsoft\Windows\CurrentVersion\Explorer\StartupApproved\Run";

            const string appName = nameof(Proxirae);

            using (RegistryKey? runKey =
                   Registry.CurrentUser.OpenSubKey(
                       runKeyPath,
                       writable: true))
            {
                runKey?.DeleteValue(
                    appName,
                    throwOnMissingValue: false);
            }

            using (RegistryKey? approvedKey =
                   Registry.CurrentUser.OpenSubKey(
                       approvedKeyPath,
                       writable: true))
            {
                approvedKey?.DeleteValue(
                    appName,
                    throwOnMissingValue: false);
            }
        }
    }
}