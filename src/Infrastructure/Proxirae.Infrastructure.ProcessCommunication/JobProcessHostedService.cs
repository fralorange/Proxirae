#pragma warning disable CA1416

using Meziantou.Framework.Win32;
using Microsoft.Extensions.Hosting;
using System.Diagnostics;

namespace Proxirae.Infrastructure.ProcessCommunication
{
    public class JobProcessHostedService : IHostedService, IDisposable
    {
        private Process? _coreProcess;
        private JobObject? _jobObject;

        public Task StartAsync(CancellationToken cancellationToken)
        {
#if DEBUG
            return Task.CompletedTask;
#endif

            _jobObject = new();

            _jobObject.SetLimits(new JobObjectLimits
            {
                Flags = JobObjectLimitFlags.KillOnJobClose
            });

            var coreExePath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "Core", "ProxiraeCore.exe");

            var startInfo = new ProcessStartInfo
            {
                FileName = coreExePath,
                UseShellExecute = false,
                CreateNoWindow = true,
            };

            _coreProcess = Process.Start(startInfo);

            if (_coreProcess != null)
            {
                _jobObject.AssignProcess(_coreProcess);
            }

            return Task.Delay(500, cancellationToken);
        }

        public Task StopAsync(CancellationToken cancellationToken)
        {
            Dispose();

            return Task.CompletedTask;
        }

        public void Dispose()
        {
            _jobObject?.Dispose();
            _coreProcess?.Dispose();

            _jobObject = null;
            _coreProcess = null;
        }
    }
}

#pragma warning restore CA1416