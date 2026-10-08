using System.Collections;
using System.Collections.Specialized;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;

namespace Proxirae.Presentation.WPF.Controls
{
    public partial class MultiComboBox : UserControl
    {
        private bool _synchronizing;

        public static readonly DependencyProperty RequireSelectionProperty =
            DependencyProperty.Register(
                nameof(RequireSelection),
                typeof(bool),
                typeof(MultiComboBox),
                new PropertyMetadata(false));

        public bool RequireSelection
        {
            get => (bool)GetValue(RequireSelectionProperty);
            set => SetValue(RequireSelectionProperty, value);
        }

        public static readonly DependencyProperty ItemsSourceProperty =
            DependencyProperty.Register(
                nameof(ItemsSource),
                typeof(IEnumerable),
                typeof(MultiComboBox),
                new PropertyMetadata(null));

        public IEnumerable? ItemsSource
        {
            get => (IEnumerable?)GetValue(ItemsSourceProperty);
            set => SetValue(ItemsSourceProperty, value);
        }

        public static readonly DependencyProperty SelectedItemsProperty =
            DependencyProperty.Register(
                nameof(SelectedItems),
                typeof(IList),
                typeof(MultiComboBox),
                new FrameworkPropertyMetadata(
                    null,
                    FrameworkPropertyMetadataOptions.BindsTwoWayByDefault,
                    OnSelectedItemsChanged));

        public IList? SelectedItems
        {
            get => (IList?)GetValue(SelectedItemsProperty);
            set => SetValue(SelectedItemsProperty, value);
        }

        public static readonly DependencyProperty SelectedValueProperty =
            DependencyProperty.Register(
                nameof(SelectedValue),
                typeof(object),
                typeof(MultiComboBox),
                new FrameworkPropertyMetadata(
                    null,
                    FrameworkPropertyMetadataOptions.BindsTwoWayByDefault,
                    OnSelectedValueChanged));

        public object? SelectedValue
        {
            get => GetValue(SelectedValueProperty);
            set => SetValue(SelectedValueProperty, value);
        }

        public static readonly DependencyProperty IsOpenProperty =
            DependencyProperty.Register(
                nameof(IsOpen),
                typeof(bool),
                typeof(MultiComboBox),
                new FrameworkPropertyMetadata(false));

        public bool IsOpen
        {
            get => (bool)GetValue(IsOpenProperty);
            set => SetValue(IsOpenProperty, value);
        }

        public static readonly DependencyProperty SelectedTextProperty =
            DependencyProperty.Register(
                nameof(SelectedText),
                typeof(string),
                typeof(MultiComboBox),
                new PropertyMetadata(string.Empty));

        public string SelectedText
        {
            get => (string)GetValue(SelectedTextProperty);
            private set => SetValue(SelectedTextProperty, value);
        }

        public bool IsDropDownMouseOver => Dropdown.IsMouseOver;

        public event SelectionChangedEventHandler? SelectionChanged;

        public MultiComboBox()
        {
            InitializeComponent();

            Loaded += OnLoaded;
            Unloaded += OnUnloaded;

            List.SelectionChanged += OnSelectionChanged;
            PreviewMouseDoubleClick += OnPreviewMouseDoubleClick;
        }

        private void OnLoaded(object sender, RoutedEventArgs e)
        {
            if (SelectedItems is INotifyCollectionChanged collection)
            {
                collection.CollectionChanged += OnSelectedItemsCollectionChanged;
            }

            InputManager.Current.PreProcessInput += OnPreProcessInput;

            SynchronizeSelection();
            UpdateSelectedText();
        }

        private void OnUnloaded(object sender, RoutedEventArgs e)
        {
            IsOpen = false;

            InputManager.Current.PreProcessInput -= OnPreProcessInput;

            if (SelectedItems is INotifyCollectionChanged collection)
            {
                collection.CollectionChanged -= OnSelectedItemsCollectionChanged;
            }
        }

        private void OnPreProcessInput(object sender, PreProcessInputEventArgs e)
        {
            if (!IsOpen ||
                e.StagingItem.Input is not MouseButtonEventArgs mouseEvent ||
                mouseEvent.ChangedButton != MouseButton.Left ||
                mouseEvent.RoutedEvent != Mouse.PreviewMouseDownEvent)
            {
                return;
            }

            if (IsMouseOver || Dropdown.IsMouseOver)
            {
                return;
            }

            IsOpen = false;
        }

        private void OnPreviewMouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            e.Handled = true;
        }

        private static void OnSelectedItemsChanged(
            DependencyObject d,
            DependencyPropertyChangedEventArgs e)
        {
            var control = (MultiComboBox)d;

            if (e.OldValue is INotifyCollectionChanged oldCollection)
            {
                oldCollection.CollectionChanged -= control.OnSelectedItemsCollectionChanged;
            }

            if (e.NewValue is INotifyCollectionChanged newCollection)
            {
                newCollection.CollectionChanged += control.OnSelectedItemsCollectionChanged;
            }

            control.SynchronizeSelection();
            control.UpdateSelectedText();
        }

        private static void OnSelectedValueChanged(
            DependencyObject d,
            DependencyPropertyChangedEventArgs e)
        {
            var control = (MultiComboBox)d;

            control.SynchronizeSelection();
            control.UpdateSelectedText();
        }

        private void OnSelectedItemsCollectionChanged(
            object? sender,
            NotifyCollectionChangedEventArgs e)
        {
            if (_synchronizing)
            {
                return;
            }

            SynchronizeSelection();
            UpdateSelectedText();
        }

        private void OnSelectionChanged(
            object sender,
            SelectionChangedEventArgs e)
        {
            if (_synchronizing)
            {
                return;
            }

            if (RequireSelection &&
                List.SelectedItems.Count == 0 &&
                e.RemovedItems.Count > 0)
            {
                _synchronizing = true;

                try
                {
                    List.SelectedItems.Add(e.RemovedItems[0]);
                }
                finally
                {
                    _synchronizing = false;
                }

                UpdateSelectedText();
                return;
            }

            if (SelectedValue is Enum selectedValue)
            {
                UpdateSelectedValue(selectedValue.GetType());
            }
            else if (SelectedItems is not null)
            {
                _synchronizing = true;

                try
                {
                    foreach (var item in e.RemovedItems)
                    {
                        SelectedItems.Remove(item);
                    }

                    foreach (var item in e.AddedItems)
                    {
                        if (!SelectedItems.Contains(item))
                        {
                            SelectedItems.Add(item);
                        }
                    }
                }
                finally
                {
                    _synchronizing = false;
                }
            }

            UpdateSelectedText();
            SelectionChanged?.Invoke(this, e);
        }

        private void SynchronizeSelection()
        {
            if (!IsLoaded)
            {
                return;
            }

            _synchronizing = true;

            try
            {
                List.SelectedItems.Clear();

                if (SelectedValue is Enum selectedValue)
                {
                    foreach (var item in List.Items)
                    {
                        if (item is Enum flag &&
                            flag.GetType() == selectedValue.GetType() &&
                            HasFlag(selectedValue, flag))
                        {
                            List.SelectedItems.Add(item);
                        }
                    }

                    return;
                }

                if (SelectedItems is null)
                {
                    return;
                }

                foreach (var item in SelectedItems)
                {
                    if (List.Items.Contains(item))
                    {
                        List.SelectedItems.Add(item);
                    }
                }
            }
            finally
            {
                _synchronizing = false;
            }
        }

        private void UpdateSelectedValue(Type enumType)
        {
            ulong value = 0;

            foreach (var item in List.SelectedItems)
            {
                if (item is Enum flag &&
                    flag.GetType() == enumType)
                {
                    value |= Convert.ToUInt64(flag);
                }
            }

            var selectedValue = Enum.ToObject(enumType, value);

            if (!Equals(SelectedValue, selectedValue))
            {
                SelectedValue = selectedValue;
            }
        }

        private static bool HasFlag(Enum value, Enum flag)
        {
            var valueBits = Convert.ToUInt64(value);
            var flagBits = Convert.ToUInt64(flag);

            return flagBits != 0 && (valueBits & flagBits) == flagBits;
        }

        private void UpdateSelectedText()
        {
            var items = List.SelectedItems.Cast<object>().ToList();

            SelectedText = string.Join(
                ", ",
                items.Select(item => item?.ToString())
                    .Where(value => !string.IsNullOrEmpty(value)));
        }
    }
}