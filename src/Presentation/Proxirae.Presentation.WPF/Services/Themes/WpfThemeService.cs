using Microsoft.Win32;
using Proxirae.Presentation.WPF.Models.Themes;
using System.Windows;

namespace Proxirae.Presentation.WPF.Services.Themes
{
    public class WpfThemeService : IThemeService, IDisposable
    {
        private const string ThemePath = "/Resources/Styles/Themes/";
        private const string BrushesPath = "/Resources/Styles/Brushes.xaml";

        private readonly ResourceDictionary _resources;
        private ResourceDictionary _themeDictionary;
        private ResourceDictionary _brushesDictionary;
        private Theme _resolvedTheme;

        public Theme Current { get; private set; }

        public WpfThemeService()
        {
            _resources = WinApp.Current.Resources;

            _themeDictionary = _resources.MergedDictionaries[0];
            _brushesDictionary = _resources.MergedDictionaries[1];

            Current = Theme.System;

            _resolvedTheme = ResolveTheme(Current);

            SystemEvents.UserPreferenceChanged += OnUserPreferenceChanged;
        }

        public void Apply(Theme theme)
        {
            var resolvedTheme = ResolveTheme(theme);

            ApplyTheme(resolvedTheme);

            Current = theme;
            _resolvedTheme = resolvedTheme;
        }

        private void ApplyTheme(Theme theme)
        {
            var themeDictionary = new ResourceDictionary
            {
                Source = new Uri($"{ThemePath}{theme}.xaml", UriKind.RelativeOrAbsolute)
            };

            var brushesDictionary = new ResourceDictionary
            {
                Source = new Uri(BrushesPath, UriKind.RelativeOrAbsolute)
            };

            var themeIndex = _resources.MergedDictionaries.IndexOf(_themeDictionary);
            var brushesIndex = _resources.MergedDictionaries.IndexOf(_brushesDictionary);

            if (themeIndex < 0)
                throw new InvalidOperationException("Theme resource dictionary was not found.");

            if (brushesIndex < 0)
                throw new InvalidOperationException("Brushes resource dictionary was not found.");

            _resources.MergedDictionaries[themeIndex] = themeDictionary;
            _resources.MergedDictionaries[brushesIndex] = brushesDictionary;

            _themeDictionary = themeDictionary;
            _brushesDictionary = brushesDictionary;
        }

        private static Theme ResolveTheme(Theme theme)
        {
            return theme == Theme.System
                ? GetSystemTheme()
                : theme;
        }

        private static Theme GetSystemTheme()
        {
            using var key = Registry.CurrentUser.OpenSubKey(
                @"Software\Microsoft\Windows\CurrentVersion\Themes\Personalize");

            var value = key?.GetValue("AppsUseLightTheme");

            return value is int intValue && intValue == 0
                ? Theme.Dark
                : Theme.Light;
        }

        private void OnUserPreferenceChanged(
            object? sender,
            UserPreferenceChangedEventArgs e)
        {
            if (Current != Theme.System)
                return;

            var systemTheme = GetSystemTheme();

            if (systemTheme == _resolvedTheme)
                return;

            WinApp.Current.Dispatcher.Invoke(() =>
            {
                if (Current == Theme.System)
                    ApplyTheme(systemTheme);

                _resolvedTheme = systemTheme;
            });
        }

        public void Dispose()
        {
            SystemEvents.UserPreferenceChanged -= OnUserPreferenceChanged;
        }
    }
}