using ZHSan.Core.Infrastructure.Configuration;
using ZHSan.Core.Presentation.UI.Services;
using ZHSan.Core.Presentation.UI.Events;
using ZHSan.Core.Presentation.UI.ViewModels;
using System.Collections.Generic;
using System.Linq;

namespace ZHSan.Core.Presentation.UI.Adapters
{
    /// <summary>
    /// Myra-overlay binding adapter scaffold for ToolBarDateRunnerPolicy diagnostics.
    /// Concrete Myra widget rendering can bind to Overlay.RecentMessages later.
    /// </summary>
    public sealed class ToolBarDateRunnerPolicyDebugOverlayAdapter : IMyraViewAdapter
    {
        private readonly ToolBarDateRunnerPolicyDebugOverlay overlay;
        private RuntimeOptions runtimeOptions;
        private readonly UiStyleTokens styleTokens;
        private ToolBarDateRunnerPolicyDebugOverlayViewModel viewModel;
        private readonly List<string> widgetLines = new List<string>();
        private List<string> lastRenderedWidgetLines = new List<string>();
        private bool widgetsCreated;
        private bool widgetDirty = true;
        private bool showCacheGroup = true;
        private bool showTransitionGroup = true;
        private bool showMiscGroup = true;
        private string currentPreset = "Debug";

        public ToolBarDateRunnerPolicyDebugOverlayAdapter(ToolBarDateRunnerPolicyDebugOverlay overlay, RuntimeOptions runtimeOptions, UiStyleTokens styleTokens)
        {
            this.overlay = overlay;
            this.runtimeOptions = runtimeOptions;
            this.styleTokens = styleTokens ?? UiStyleTokens.Default();
            this.viewModel = BuildDefaultViewModel(this.styleTokens);
            this.ApplyDefaultGroupVisibilityFromOptions();
        }

        public void UpdateRuntimeOptions(RuntimeOptions options)
        {
            if (options == null) return;
            this.runtimeOptions = options;
            this.ApplyDefaultGroupVisibilityFromOptions();
            this.widgetDirty = true;
        }

        public string ViewId => "toolbar-date-runner-policy-debug-overlay";

        public bool IsVisible { get; private set; }

        public void Show()
        {
            this.EnsureWidgetsCreated();
            this.IsVisible = true;
        }

        public void Hide()
        {
            this.IsVisible = false;
            if (this.runtimeOptions?.Debug?.ShowDebugOverlay != true)
            {
                this.ReleaseWidgets();
            }
        }

        public void Bind(object viewModel)
        {
            this.viewModel = viewModel as ToolBarDateRunnerPolicyDebugOverlayViewModel ?? this.viewModel;
            this.widgetDirty = true;
        }

        public void Refresh()
        {
            var hasMessages = this.overlay != null && this.overlay.RecentMessages.Count > 0;
            if (this.runtimeOptions?.Debug?.ShowDebugOverlay == true)
            {
                this.UpdateViewModelMessages();
                this.Show();
                return;
            }

            if (!hasMessages)
            {
                this.Hide();
                return;
            }

            this.Hide();
        }

        public void Dispose()
        {
            // overlay lifecycle owned by runtime service registry
        }

        public ToolBarDateRunnerPolicyDebugOverlayViewModel SnapshotViewModel()
        {
            this.UpdateViewModelMessages();
            return this.viewModel;
        }

        public IReadOnlyList<string> SnapshotWidgetLines()
        {
            this.UpdateViewModelMessages();
            this.widgetLines.Clear();
            if (this.viewModel?.Messages == null) return this.widgetLines;
            var cacheLines = this.viewModel.Messages.Where(message => message.StartsWith("[Cache]", System.StringComparison.OrdinalIgnoreCase)).ToList();
            var transitionLines = this.viewModel.Messages.Where(message => message.StartsWith("[Transition]", System.StringComparison.OrdinalIgnoreCase)).ToList();
            var miscLines = this.viewModel.Messages.Where(message =>
                !message.StartsWith("[Cache]", System.StringComparison.OrdinalIgnoreCase)
                && !message.StartsWith("[Transition]", System.StringComparison.OrdinalIgnoreCase)).ToList();

            if (this.showCacheGroup)
            {
                this.widgetLines.Add("== Cache ==");
                this.widgetLines.AddRange(cacheLines.Select(message => "• " + message));
            }

            if (this.showTransitionGroup)
            {
                this.widgetLines.Add("== Transition ==");
                this.widgetLines.AddRange(transitionLines.Select(message => "• " + message));
            }
            if (this.showMiscGroup)
            {
                if (this.viewModel.PersistenceAlertLines != null && this.viewModel.PersistenceAlertLines.Count > 0)
                {
                    this.widgetLines.Add("== PersistenceAlert ==");
                    this.widgetLines.AddRange(this.viewModel.PersistenceAlertLines.Select(message => "• " + message));
                }

                this.widgetLines.Add("== Misc ==");
                this.widgetLines.AddRange(miscLines.Select(message => "• " + message));
            }
            this.widgetDirty = this.widgetDirty || !this.AreSameLines(this.widgetLines, this.lastRenderedWidgetLines);
            return this.widgetLines;
        }

        public void HandleMouseWheelDelta(int wheelDelta)
        {
            if (wheelDelta == 0 || this.viewModel == null)
            {
                return;
            }

            var step = wheelDelta > 0 ? -1 : 1;
            var totalMessages = this.overlay?.RecentMessages?.Count ?? 0;
            var maxVisible = this.viewModel.MaxVisibleLines <= 0 ? 10 : this.viewModel.MaxVisibleLines;
            var maxOffset = totalMessages > maxVisible ? totalMessages - maxVisible : 0;
            var nextOffset = this.viewModel.ScrollOffset + step;
            if (nextOffset < 0) nextOffset = 0;
            if (nextOffset > maxOffset) nextOffset = maxOffset;
            this.viewModel.ScrollOffset = nextOffset;
            this.UpdateViewModelMessages();
            this.widgetDirty = true;
        }

        private void UpdateViewModelMessages()
        {
            if (this.viewModel == null)
            {
                this.viewModel = BuildDefaultViewModel(this.styleTokens);
            }

            var all = this.overlay?.RecentMessages?.ToList() ?? new List<string>();
            var maxVisible = this.viewModel.MaxVisibleLines <= 0 ? 10 : this.viewModel.MaxVisibleLines;
            var skip = this.viewModel.ScrollOffset;
            if (skip < 0) skip = 0;
            if (skip > all.Count) skip = all.Count;
            this.viewModel.Messages = all.Skip(skip).Take(maxVisible).ToList();
            this.viewModel.PersistenceAlertSnapshot = this.overlay?.LatestPersistenceAlertSnapshot;
            this.viewModel.PersistenceAlertLines = this.BuildPersistenceAlertLines(this.viewModel.PersistenceAlertSnapshot);
            this.widgetDirty = true;
        }

        private List<string> BuildPersistenceAlertLines(RuntimeOptionsPersistenceAlertSnapshot snapshot)
        {
            var lines = new List<string>();
            if (snapshot == null)
            {
                return lines;
            }

            lines.Add(string.Format(
                "Level={0}, Trigger={1}, Consecutive={2}/{3}, QuietWindowMs={4}",
                snapshot.Level,
                snapshot.Trigger,
                snapshot.ConsecutiveFailureCount,
                snapshot.Threshold,
                snapshot.QuietWindowMs));
            lines.Add(string.Format(
                "ThresholdHits={0}, EmittedAlerts={1}, QuietSuppressed={2}",
                snapshot.ThresholdHitCount,
                snapshot.EmittedAlertCount,
                snapshot.QuietWindowSuppressedCount));
            if (snapshot.Attempt > 0 || snapshot.MaxAttempts > 0)
            {
                lines.Add(string.Format(
                    "Attempt={0}/{1}, Path={2}, Error={3}",
                    snapshot.Attempt,
                    snapshot.MaxAttempts,
                    snapshot.Path,
                    snapshot.ErrorType));
            }

            if (!string.IsNullOrWhiteSpace(snapshot.Message))
            {
                lines.Add(snapshot.Message);
            }

            return lines;
        }

        private static ToolBarDateRunnerPolicyDebugOverlayViewModel BuildDefaultViewModel(UiStyleTokens styleTokens)
        {
            return new ToolBarDateRunnerPolicyDebugOverlayViewModel
            {
                FontSize = styleTokens.BaseFontSize,
                LineSpacing = styleTokens.Spacing,
                TextColor = styleTokens.PrimaryTextColor
            };
        }

        public bool RefreshWidgetsIfNeeded()
        {
            this.EnsureWidgetsCreated();
            var lines = this.SnapshotWidgetLines();
            if (!this.widgetDirty && this.AreSameLines(lines, this.lastRenderedWidgetLines))
            {
                return false;
            }

            this.lastRenderedWidgetLines = lines.ToList();
            this.widgetDirty = false;
            return true;
        }

        public void ReleaseWidgets()
        {
            this.widgetsCreated = false;
            this.lastRenderedWidgetLines = new List<string>();
            this.widgetDirty = true;
        }

        private void EnsureWidgetsCreated()
        {
            if (this.widgetsCreated) return;
            this.widgetsCreated = true;
            this.widgetDirty = true;
        }

        private bool AreSameLines(IReadOnlyList<string> left, IReadOnlyList<string> right)
        {
            if (left == null && right == null) return true;
            if (left == null || right == null) return false;
            if (left.Count != right.Count) return false;
            for (var i = 0; i < left.Count; i++)
            {
                if (!string.Equals(left[i], right[i], System.StringComparison.Ordinal))
                {
                    return false;
                }
            }

            return true;
        }

        public string ToggleGroupVisibility()
        {
            if (this.showCacheGroup && this.showTransitionGroup)
            {
                this.showTransitionGroup = false;
            }
            else if (this.showCacheGroup && !this.showTransitionGroup)
            {
                this.showCacheGroup = false;
                this.showTransitionGroup = true;
            }
            else
            {
                this.showCacheGroup = true;
                this.showTransitionGroup = true;
                this.showMiscGroup = true;
            }

            this.widgetDirty = true;
            return this.GetGroupModeDescription();
        }

        public string GetGroupModeDescription()
        {
            return string.Format(
                "Groups: Cache={0}, Transition={1}, Misc={2}",
                this.showCacheGroup ? "ON" : "OFF",
                this.showTransitionGroup ? "ON" : "OFF",
                this.showMiscGroup ? "ON" : "OFF");
        }

        private void ApplyDefaultGroupVisibilityFromOptions()
        {
            var policy = this.runtimeOptions?.Ui?.ToolBarDateRunnerPolicy;
            if (policy == null) return;
            this.showCacheGroup = policy.DiagnosticsOverlayShowCacheGroupByDefault;
            this.showTransitionGroup = policy.DiagnosticsOverlayShowTransitionGroupByDefault;
            this.showMiscGroup = policy.DiagnosticsOverlayShowMiscGroupByDefault;
            this.ApplyPreset(policy.DiagnosticsOverlayPreset);
            if (!this.showCacheGroup && !this.showTransitionGroup && !this.showMiscGroup)
            {
                this.showCacheGroup = true;
            }
        }

        public string CyclePreset()
        {
            if (string.Equals(this.currentPreset, "Performance", System.StringComparison.OrdinalIgnoreCase))
            {
                this.ApplyPreset("Debug");
            }
            else if (string.Equals(this.currentPreset, "Debug", System.StringComparison.OrdinalIgnoreCase))
            {
                this.ApplyPreset("Minimal");
            }
            else
            {
                this.ApplyPreset("Performance");
            }

            this.widgetDirty = true;
            return this.currentPreset;
        }

        private void ApplyPreset(string preset)
        {
            if (string.Equals(preset, "Performance", System.StringComparison.OrdinalIgnoreCase))
            {
                this.showCacheGroup = true;
                this.showTransitionGroup = false;
                this.showMiscGroup = false;
                this.currentPreset = "Performance";
                return;
            }

            if (string.Equals(preset, "Minimal", System.StringComparison.OrdinalIgnoreCase))
            {
                this.showCacheGroup = false;
                this.showTransitionGroup = true;
                this.showMiscGroup = false;
                this.currentPreset = "Minimal";
                return;
            }

            this.showCacheGroup = true;
            this.showTransitionGroup = true;
            this.showMiscGroup = true;
            this.currentPreset = "Debug";
        }
    }
}
