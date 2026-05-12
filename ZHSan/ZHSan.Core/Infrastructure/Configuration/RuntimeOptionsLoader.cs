using System.IO;
using System.Text.Json;
using System;
using Microsoft.Xna.Framework.Input;

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
            if (options.Input == null)
            {
                options.Input = new InputOptions();
            }

            if (options.Ui.TabListProfiles == null)
            {
                options.Ui.TabListProfiles = new TabListProfileOptions();
            }

            if (options.Ui.ToolBarDateRunnerPolicy == null)
            {
                log?.Invoke("[RuntimeOptionsLoader] ToolBarDateRunnerPolicy is null. Fallback to defaults.");
                options.Ui.ToolBarDateRunnerPolicy = new ToolBarDateRunnerPolicyOptions();
            }
            else
            {
                if (options.Ui.ToolBarDateRunnerPolicy.CacheDiagnosticsLogSampleEveryInvalidations <= 0)
                {
                    log?.Invoke("[RuntimeOptionsLoader] Invalid cacheDiagnosticsLogSampleEveryInvalidations detected. Fallback to 20.");
                    options.Ui.ToolBarDateRunnerPolicy.CacheDiagnosticsLogSampleEveryInvalidations = 20;
                }

                if (options.Ui.ToolBarDateRunnerPolicy.CacheDiagnosticsLogMinIntervalMs < 0)
                {
                    log?.Invoke("[RuntimeOptionsLoader] Invalid cacheDiagnosticsLogMinIntervalMs detected. Fallback to 5000.");
                    options.Ui.ToolBarDateRunnerPolicy.CacheDiagnosticsLogMinIntervalMs = 5000;
                }

                if (options.Ui.ToolBarDateRunnerPolicy.DiagnosticsOverlayMaxMessages <= 0)
                {
                    log?.Invoke("[RuntimeOptionsLoader] Invalid diagnosticsOverlayMaxMessages detected. Fallback to 30.");
                    options.Ui.ToolBarDateRunnerPolicy.DiagnosticsOverlayMaxMessages = 30;
                }
                if (options.Ui.ToolBarDateRunnerPolicy.DiagnosticsOverlayMaxVisibleLinesPerGroup <= 0)
                {
                    log?.Invoke("[RuntimeOptionsLoader] Invalid diagnosticsOverlayMaxVisibleLinesPerGroup detected. Fallback to 6.");
                    options.Ui.ToolBarDateRunnerPolicy.DiagnosticsOverlayMaxVisibleLinesPerGroup = 6;
                }
                if (options.Ui.ToolBarDateRunnerPolicy.DiagnosticsOverlayPersistenceRetryCount < 0)
                {
                    log?.Invoke("[RuntimeOptionsLoader] Invalid diagnosticsOverlayPersistenceRetryCount detected. Fallback to 1.");
                    options.Ui.ToolBarDateRunnerPolicy.DiagnosticsOverlayPersistenceRetryCount = 1;
                }
                if (options.Ui.ToolBarDateRunnerPolicy.DiagnosticsOverlayPersistenceAlertThreshold <= 0)
                {
                    log?.Invoke("[RuntimeOptionsLoader] Invalid diagnosticsOverlayPersistenceAlertThreshold detected. Fallback to 1.");
                    options.Ui.ToolBarDateRunnerPolicy.DiagnosticsOverlayPersistenceAlertThreshold = 1;
                }
                if (options.Ui.ToolBarDateRunnerPolicy.DiagnosticsOverlayPersistenceAlertQuietWindowMs < 0)
                {
                    log?.Invoke("[RuntimeOptionsLoader] Invalid diagnosticsOverlayPersistenceAlertQuietWindowMs detected. Fallback to 0.");
                    options.Ui.ToolBarDateRunnerPolicy.DiagnosticsOverlayPersistenceAlertQuietWindowMs = 0;
                }
                if (options.Ui.ToolBarDateRunnerPolicy.InputDiagnosticsMinIntervalMs < 0)
                {
                    log?.Invoke("[RuntimeOptionsLoader] Invalid inputDiagnosticsMinIntervalMs detected. Fallback to 150.");
                    options.Ui.ToolBarDateRunnerPolicy.InputDiagnosticsMinIntervalMs = 150;
                }
                if (options.Ui.ToolBarDateRunnerPolicy.InputDiagnosticsTrendWindowSeconds <= 0)
                {
                    log?.Invoke("[RuntimeOptionsLoader] Invalid inputDiagnosticsTrendWindowSeconds detected. Fallback to 10.");
                    options.Ui.ToolBarDateRunnerPolicy.InputDiagnosticsTrendWindowSeconds = 10;
                }
                if (options.Ui.ToolBarDateRunnerPolicy.InputDiagnosticsFirstSampleHintSeconds <= 0)
                {
                    log?.Invoke("[RuntimeOptionsLoader] Invalid inputDiagnosticsFirstSampleHintSeconds detected. Fallback to 3.");
                    options.Ui.ToolBarDateRunnerPolicy.InputDiagnosticsFirstSampleHintSeconds = 3;
                }

                var category = options.Ui.ToolBarDateRunnerPolicy.DiagnosticsOverlayMinimumCategory;
                if (!string.IsNullOrWhiteSpace(category))
                {
                    var normalized = category.Trim();
                    if (!string.Equals(normalized, "cache", StringComparison.OrdinalIgnoreCase)
                        && !string.Equals(normalized, "transition", StringComparison.OrdinalIgnoreCase))
                    {
                        log?.Invoke("[RuntimeOptionsLoader] Invalid diagnosticsOverlayMinimumCategory detected. Fallback to Cache.");
                        options.Ui.ToolBarDateRunnerPolicy.DiagnosticsOverlayMinimumCategory = "Cache";
                    }
                }
            }

            options.Input.DebugOverlayToggleKey = NormalizeKeyBinding(options.Input.DebugOverlayToggleKey, "F10", "debugOverlayToggleKey", log);
            options.Input.DebugOverlayGroupToggleKey = NormalizeKeyBinding(options.Input.DebugOverlayGroupToggleKey, "F11", "debugOverlayGroupToggleKey", log);
            options.Input.DebugOverlayReloadKey = NormalizeKeyBinding(options.Input.DebugOverlayReloadKey, "F9", "debugOverlayReloadKey", log);
            options.Input.DebugOverlayPresetCycleKey = NormalizeKeyBinding(options.Input.DebugOverlayPresetCycleKey, "F8", "debugOverlayPresetCycleKey", log);
            options.Input.DebugOverlayScrollGroupKey = NormalizeKeyBinding(options.Input.DebugOverlayScrollGroupKey, "F7", "debugOverlayScrollGroupKey", log);
            options.Ui.ToolBarDateRunnerPolicy.DiagnosticsOverlayPreset = NormalizeOverlayPreset(options.Ui.ToolBarDateRunnerPolicy.DiagnosticsOverlayPreset, log);

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

        private static string NormalizeKeyBinding(string value, string fallback, string fieldName, Action<string> log)
        {
            if (string.IsNullOrWhiteSpace(value))
            {
                return fallback;
            }

            Keys parsed;
            if (Enum.TryParse(value.Trim(), true, out parsed))
            {
                return parsed.ToString();
            }

            log?.Invoke(string.Format("[RuntimeOptionsLoader] Invalid {0} detected. Fallback to {1}.", fieldName, fallback));
            return fallback;
        }

        private static string NormalizeOverlayPreset(string value, Action<string> log)
        {
            if (string.IsNullOrWhiteSpace(value)) return "Debug";
            var normalized = value.Trim();
            if (string.Equals(normalized, "Performance", StringComparison.OrdinalIgnoreCase)
                || string.Equals(normalized, "Debug", StringComparison.OrdinalIgnoreCase)
                || string.Equals(normalized, "Minimal", StringComparison.OrdinalIgnoreCase))
            {
                return normalized;
            }

            log?.Invoke("[RuntimeOptionsLoader] Invalid diagnosticsOverlayPreset detected. Fallback to Debug.");
            return "Debug";
        }
    }
}
