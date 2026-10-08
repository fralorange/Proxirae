namespace Proxirae.Presentation.WPF
{
    public sealed class StartupOptions
    {
        public bool Silent { get; init; }
        public bool Minimized { get; init; }
        public bool Autostart { get; init; }

        public static StartupOptions Parse(string[] args)
        {
            return new StartupOptions
            {
                Silent = args.Any(x =>
                    string.Equals(x, "--silent", StringComparison.OrdinalIgnoreCase)),

                Minimized = args.Any(x =>
                    string.Equals(x, "--minimized", StringComparison.OrdinalIgnoreCase)),

                Autostart = args.Any(x =>
                    string.Equals(x, "--autostart", StringComparison.OrdinalIgnoreCase))
            };
        }
    }
}