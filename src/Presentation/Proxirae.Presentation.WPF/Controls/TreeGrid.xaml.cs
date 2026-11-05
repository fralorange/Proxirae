using System.Collections;
using System.Collections.ObjectModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;

namespace Proxirae.Presentation.WPF.Controls
{
    public partial class TreeGrid : UserControl
    {
        private CollectionViewSource? _groupedView;
        public TreeGrid()
        {
            InitializeComponent();
        }

        #region Public API

        public static readonly DependencyProperty DataGridStyleProperty =
            DependencyProperty.Register(
                nameof(DataGridStyle),
                typeof(Style),
                typeof(TreeGrid),
                new PropertyMetadata(null));

        public Style DataGridStyle
        {
            get => (Style)GetValue(DataGridStyleProperty);
            set => SetValue(DataGridStyleProperty, value);
        }

        public static readonly DependencyProperty GroupExpanderStyleProperty =
            DependencyProperty.Register(
                nameof(GroupExpanderStyle),
                typeof(Style),
                typeof(TreeGrid),
                new PropertyMetadata(null));

        public Style GroupExpanderStyle
        {
            get => (Style)GetValue(GroupExpanderStyleProperty);
            set => SetValue(GroupExpanderStyleProperty, value);
        }


        public static readonly DependencyProperty GroupNameStyleProperty =
            DependencyProperty.Register(
                nameof(GroupNameStyle),
                typeof(Style),
                typeof(TreeGrid),
                new PropertyMetadata(null));

        public Style GroupNameStyle
        {
            get => (Style)GetValue(GroupNameStyleProperty);
            set => SetValue(GroupNameStyleProperty, value);
        }


        public static readonly DependencyProperty GroupCountStyleProperty =
            DependencyProperty.Register(
                nameof(GroupCountStyle),
                typeof(Style),
                typeof(TreeGrid),
                new PropertyMetadata(null));

        public Style GroupCountStyle
        {
            get => (Style)GetValue(GroupCountStyleProperty);
            set => SetValue(GroupCountStyleProperty, value);
        }


        public static readonly DependencyProperty GroupPlaceholderStyleProperty =
            DependencyProperty.Register(
                nameof(GroupPlaceholderStyle),
                typeof(Style),
                typeof(TreeGrid),
                new PropertyMetadata(null));

        public Style GroupPlaceholderStyle
        {
            get => (Style)GetValue(GroupPlaceholderStyleProperty);
            set => SetValue(GroupPlaceholderStyleProperty, value);
        }

        public static readonly DependencyProperty ItemsSourceProperty =
            DependencyProperty.Register(
                nameof(ItemsSource),
                typeof(IEnumerable),
                typeof(TreeGrid),
                new PropertyMetadata(null, OnItemsSourceChanged));


        public IEnumerable ItemsSource
        {
            get => (IEnumerable)GetValue(ItemsSourceProperty);
            set => SetValue(ItemsSourceProperty, value);
        }

        public static readonly DependencyProperty GroupByProperty =
            DependencyProperty.Register(
                nameof(GroupBy),
                typeof(string),
                typeof(TreeGrid),
                new PropertyMetadata(null, OnGroupByChanged));


        public string GroupBy
        {
            get => (string)GetValue(GroupByProperty);
            set => SetValue(GroupByProperty, value);
        }

        public ObservableCollection<DataGridColumn> Columns
            => PART_DataGrid.Columns;

        public DataGrid InnerDataGrid
            => PART_DataGrid;

        public static readonly DependencyProperty SelectedItemProperty =
            DependencyProperty.Register(
                nameof(SelectedItem),
                typeof(object),
                typeof(TreeGrid),
                new FrameworkPropertyMetadata(
                    null,
                    FrameworkPropertyMetadataOptions.BindsTwoWayByDefault));


        public object SelectedItem
        {
            get => GetValue(SelectedItemProperty);
            set => SetValue(SelectedItemProperty, value);
        }

        public IList SelectedItems
            => PART_DataGrid.SelectedItems;


        #endregion

        #region Grouping

        private static void OnItemsSourceChanged(
            DependencyObject d,
            DependencyPropertyChangedEventArgs e)
        {
            var control = (TreeGrid)d;

            control.RefreshView();
        }


        private static void OnGroupByChanged(
            DependencyObject d,
            DependencyPropertyChangedEventArgs e)
        {
            var control = (TreeGrid)d;

            control.RefreshView();
        }

        private void RefreshView()
        {
            if (PART_DataGrid == null)
                return;

            _groupedView = null;

            if (ItemsSource == null)
            {
                PART_DataGrid.ItemsSource = null;
                return;
            }

            if (string.IsNullOrWhiteSpace(GroupBy))
            {
                PART_DataGrid.ItemsSource = ItemsSource;
                return;
            }

            var viewSource = new CollectionViewSource
            {
                Source = ItemsSource
            };

            viewSource.GroupDescriptions.Add(
                new PropertyGroupDescription(GroupBy));

            _groupedView = viewSource;

            PART_DataGrid.ItemsSource = _groupedView.View;
        }

        public void RefreshGroups()
        {
            _groupedView?.View?.Refresh();
        }

        #endregion
    }
}