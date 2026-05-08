using System.IO;
using System.Text.Json;
using System;

namespace ZHSan.Core.Infrastructure.Configuration
{
    public static class RuntimeOptionsLoader
    {
        public static RuntimeOptions LoadOrDefault(string filePath, Action<string> log = null)
        {
            if (string.IsNullOrWhiteSpace(filePath) || !File.Exists(filePath))
            {
                return RuntimeOptions.Default();
            }

            try
            {
                var json = File.ReadAllText(filePath);
                var options = JsonSerializer.Deserialize<RuntimeOptions>(json, new JsonSerializerOptions
                {
                    PropertyNameCaseInsensitive = true
                });
                var normalized = Normalize(options, log);
                return normalized ?? RuntimeOptions.Default();
            }
            catch
            {
                log?.Invoke("[RuntimeOptionsLoader] Failed to load runtime options. Fallback to defaults.");
                return RuntimeOptions.Default();
            }
        }

        private static RuntimeOptions Normalize(RuntimeOptions options, Action<string> log)
        {
            if (options == null)
            {
                log?.Invoke("[RuntimeOptionsLoader] Runtime options is null. Fallback to defaults.");
                return RuntimeOptions.Default();
            }

            if (options.Ui == null)
            {
                options.Ui = new UiOptions();
            }

            if (options.Ui.TabListProfiles == null)
            {
                options.Ui.TabListProfiles = new TabListProfileOptions();
            }

            if (options.Ui.TabListProfiles.Overrides == null)
            {
                options.Ui.TabListProfiles.Overrides = new System.Collections.Generic.List<TabListProfileOverrideOption>();
            }

            foreach (var item in options.Ui.TabListProfiles.Overrides)
            {
                if (item == null) continue;
                if (item.PageSize.HasValue && item.PageSize.Value <= 0)
                {
                    log?.Invoke("[RuntimeOptionsLoader] Invalid pageSize detected. Reset to null.");
                    item.PageSize = null;
                }

                item.NameFilterMode = NormalizeMatchMode(item.NameFilterMode, log);
                item.StatusFilterMode = NormalizeMatchMode(item.StatusFilterMode, log);
            }

            return options;
        }

        private static string NormalizeMatchMode(string mode, Action<string> log)
        {
            if (string.IsNullOrWhiteSpace(mode)) return mode;
            var normalized = mode.Trim().ToLowerInvariant();
            if (normalized == "contains" || normalized == "exact" || normalized == "prefix")
            {
                return normalized;
            }

            log?.Invoke("[RuntimeOptionsLoader] Invalid match mode detected. Fallback to contains.");
            return "contains";
        }
    }
}
