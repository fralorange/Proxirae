namespace Proxirae.Presentation.WPF
{
    public static class ProgramInfo
    {
        public const string AppName = "Proxirae";
        public const string AppVersion = "0.0.2";
        public static string AppBitness => Environment.Is64BitProcess ? "x64" : "x32";
    }
}
