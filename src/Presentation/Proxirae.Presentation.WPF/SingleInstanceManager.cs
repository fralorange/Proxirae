namespace Proxirae.Presentation.WPF
{
    public class SingleInstanceManager : IDisposable
    {
        private Mutex? _mutex;
        private readonly bool _isFirstInstance;

        public bool IsFirstInstance => _isFirstInstance;

        public SingleInstanceManager(string appId)
        {
            _mutex = new(true, appId, out _isFirstInstance);
        }

        public void Dispose()
        {
            if (_isFirstInstance && _mutex is not null)
            {
                _mutex.ReleaseMutex();
                _mutex.Dispose();
                _mutex = null;
            }
        }
    }
}
