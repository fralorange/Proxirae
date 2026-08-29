namespace Proxirae.Presentation.WPF
{
    internal class Program
    {
        [STAThread]
        public static void Main()
        {
            using var instanceManager = new SingleInstanceManager(nameof(Proxirae));

            if (!instanceManager.IsFirstInstance)
            {
                return;
            }

            var app = new App();
            app.InitializeComponent();
            app.Run();
        }
    }
}
