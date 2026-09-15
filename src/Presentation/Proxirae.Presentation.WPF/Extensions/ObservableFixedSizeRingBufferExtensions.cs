using ObservableCollections;

namespace Proxirae.Presentation.WPF.Extensions
{
    public static class ObservableFixedSizeRingBufferExtensions
    {
        public static (ObservableFixedSizeRingBuffer<T> Buffer, INotifyCollectionChangedSynchronizedViewList<T> View) Resize<T>(
            this ObservableFixedSizeRingBuffer<T> oldBuffer,
            IDisposable? oldViewList,
            int newCapacity)
        {
            var itemsToKeep = oldBuffer.TakeLast(newCapacity).ToList();

            var newBuffer = new ObservableFixedSizeRingBuffer<T>(newCapacity);
            foreach (var item in itemsToKeep)
            {
                newBuffer.AddLast(item);
            }

            var newView = newBuffer
                .CreateView(x => x)
                .ToNotifyCollectionChanged();

            oldViewList?.Dispose();

            return (newBuffer, newView);
        }
    }
}
