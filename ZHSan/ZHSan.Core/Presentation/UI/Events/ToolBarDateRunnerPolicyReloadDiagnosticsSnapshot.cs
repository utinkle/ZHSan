using System;

namespace ZHSan.Core.Presentation.UI.Events
{
    public sealed class ToolBarDateRunnerPolicyReloadDiagnosticsSnapshot
    {
        public ToolBarDateRunnerPolicyReloadDiagnosticsSnapshot(string source, bool debugOverlayEnabled)
        {
            this.Source = string.IsNullOrWhiteSpace(source) ? "unknown" : source;
            this.DebugOverlayEnabled = debugOverlayEnabled;
        }

        public string Source { get; }
        public bool DebugOverlayEnabled { get; }
    }
}
