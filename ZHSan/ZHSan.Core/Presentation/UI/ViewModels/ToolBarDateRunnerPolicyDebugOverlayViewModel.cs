using System.Collections.Generic;
using Microsoft.Xna.Framework;
using ZHSan.Core.Presentation.UI.Events;

namespace ZHSan.Core.Presentation.UI.ViewModels
{
    public sealed class ToolBarDateRunnerPolicyDebugOverlayViewModel
    {
        public List<string> Messages { get; set; } = new List<string>();

        public List<ToolBarDateRunnerPolicyDiagnosticsEntry> Entries { get; set; } = new List<ToolBarDateRunnerPolicyDiagnosticsEntry>();

        public List<ToolBarDateRunnerPolicyDiagnosticsEntry> CacheEntries { get; set; } = new List<ToolBarDateRunnerPolicyDiagnosticsEntry>();

        public List<ToolBarDateRunnerPolicyDiagnosticsEntry> TransitionEntries { get; set; } = new List<ToolBarDateRunnerPolicyDiagnosticsEntry>();

        public List<ToolBarDateRunnerPolicyDiagnosticsEntry> MiscEntries { get; set; } = new List<ToolBarDateRunnerPolicyDiagnosticsEntry>();

        public RuntimeOptionsPersistenceAlertSnapshot PersistenceAlertSnapshot { get; set; }

        public ToolBarDateRunnerPolicyCacheDiagnosticsSnapshot CacheSnapshot { get; set; }

        public ToolBarDateRunnerPolicyTransitionDiagnosticsSnapshot TransitionSnapshot { get; set; }

        public ToolBarDateRunnerPolicyReloadDiagnosticsSnapshot ReloadSnapshot { get; set; }

        public ToolBarDateRunnerPolicyInputDiagnosticsSnapshot InputSnapshot { get; set; }

        public List<string> PersistenceAlertLines { get; set; } = new List<string>();

        public List<string> CacheLines { get; set; } = new List<string>();

        public List<string> TransitionLines { get; set; } = new List<string>();

        public List<string> ReloadLines { get; set; } = new List<string>();

        public List<string> InputLines { get; set; } = new List<string>();

        public int FontSize { get; set; }

        public int LineSpacing { get; set; }

        public Color TextColor { get; set; }

        public float BackgroundOpacity { get; set; } = 0.65f;

        public int ScrollOffset { get; set; }

        public int CacheScrollOffset { get; set; }

        public int TransitionScrollOffset { get; set; }

        public int MiscScrollOffset { get; set; }

        public int MaxVisibleLines { get; set; } = 10;

        public int MaxVisibleLinesPerGroup { get; set; } = 6;
    }
}
