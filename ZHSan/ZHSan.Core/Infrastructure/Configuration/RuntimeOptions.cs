namespace ZHSan.Core.Infrastructure.Configuration
{
    /// <summary>
    /// Unified runtime options entry for phased migration.
    /// </summary>
    public sealed class RuntimeOptions
    {
        public GraphicsOptions Graphics { get; set; } = new GraphicsOptions();

        public UiOptions Ui { get; set; } = new UiOptions();

        public EffectsOptions Effects { get; set; } = new EffectsOptions();

        public InputOptions Input { get; set; } = new InputOptions();

        public DebugOptions Debug { get; set; } = new DebugOptions();

        public static RuntimeOptions Default() => new RuntimeOptions();
    }

    public sealed class GraphicsOptions
    {
        public bool IsFullScreen { get; set; }

        public float RenderScale { get; set; } = 1f;
    }

    public sealed class UiOptions
    {
        public float UiScale { get; set; } = 1f;

        public TabListProfileOptions TabListProfiles { get; set; } = new TabListProfileOptions();

        public ToolBarDateRunnerPolicyOptions ToolBarDateRunnerPolicy { get; set; } = new ToolBarDateRunnerPolicyOptions();
    }

    public sealed class ToolBarDateRunnerPolicyOptions
    {
        public bool LockInNoneFlow { get; set; } = true;
        public bool LockInMapViewSelector { get; set; } = true;
        public bool LockInSelector { get; set; } = true;
        public bool LockInDialog { get; set; } = true;
        public bool SuspendOnOptionDialog { get; set; } = true;
        public bool EnableCacheDiagnosticsLog { get; set; } = true;
        public int CacheDiagnosticsLogSampleEveryInvalidations { get; set; } = 20;
        public int CacheDiagnosticsLogMinIntervalMs { get; set; } = 5000;
        public bool EnablePolicyTransitionDebugLog { get; set; } = false;
        public int DiagnosticsOverlayMaxMessages { get; set; } = 30;
        public int DiagnosticsOverlayMaxVisibleLinesPerGroup { get; set; } = 6;
        public string DiagnosticsOverlayMinimumCategory { get; set; } = "Cache";
        public bool DiagnosticsOverlayShowCacheGroupByDefault { get; set; } = true;
        public bool DiagnosticsOverlayShowTransitionGroupByDefault { get; set; } = true;
        public bool DiagnosticsOverlayShowMiscGroupByDefault { get; set; } = true;
        public string DiagnosticsOverlayPreset { get; set; } = "Debug";
        public bool DiagnosticsOverlayPersistenceRetryEnabled { get; set; } = true;
        public int DiagnosticsOverlayPersistenceRetryCount { get; set; } = 1;
        public int DiagnosticsOverlayPersistenceAlertThreshold { get; set; } = 1;
        public int DiagnosticsOverlayPersistenceAlertQuietWindowMs { get; set; } = 0;
    }

    public sealed class TabListProfileOptions
    {
        public System.Collections.Generic.List<TabListProfileOverrideOption> Overrides { get; set; } = new System.Collections.Generic.List<TabListProfileOverrideOption>();
    }

    public sealed class TabListProfileOverrideOption
    {
        public string FrameKind { get; set; }
        public string FrameFunction { get; set; }
        public int? PageSize { get; set; }
        public string NameFilterMode { get; set; }
        public string StatusFilterMode { get; set; }
        public System.Collections.Generic.List<TabListColumnOverrideOption> Columns { get; set; } = new System.Collections.Generic.List<TabListColumnOverrideOption>();
        public TabListSortOverrideOption Sort { get; set; }
    }

    public sealed class TabListColumnOverrideOption
    {
        public string Id { get; set; }
        public string Title { get; set; }
        public int Width { get; set; }
        public bool? Visible { get; set; }
        public bool? Numeric { get; set; }
        public bool? SmallToBig { get; set; }
    }

    public sealed class TabListSortOverrideOption
    {
        public string ColumnId { get; set; }
        public bool Descending { get; set; }
    }

    public sealed class EffectsOptions
    {
        public bool EnableParticles { get; set; } = true;

        public bool EnableCameraShake { get; set; } = true;

        public bool EnableDamageFlash { get; set; } = true;
    }

    public sealed class InputOptions
    {
        public bool InvertMouseWheelZoom { get; set; }
        public string DebugOverlayToggleKey { get; set; } = "F10";
        public string DebugOverlayGroupToggleKey { get; set; } = "F11";
        public string DebugOverlayReloadKey { get; set; } = "F9";
        public string DebugOverlayPresetCycleKey { get; set; } = "F8";
        public string DebugOverlayScrollGroupKey { get; set; } = "F7";
    }

    public sealed class DebugOptions
    {
        public bool ShowFps { get; set; }

        public bool ShowDebugOverlay { get; set; }
    }
}
