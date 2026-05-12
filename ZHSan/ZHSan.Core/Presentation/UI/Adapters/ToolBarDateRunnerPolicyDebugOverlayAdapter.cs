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
        private string activeScrollGroup = "Cache";

        public ToolBarDateRunnerPolicyDebugOverlayAdapter(ToolBarDateRunnerPolicyDebugOverlay overlay, RuntimeOptions runtimeOptions, UiStyleTokens styleTokens)
        {
            this.overlay = overlay;
            this.runtimeOptions = runtimeOptions;
            this.styleTokens = styleTokens ?? UiStyleTokens.Default();
            this.viewModel = BuildDefaultViewModel(this.styleTokens);
            this.ApplyDefaultGroupVisibilityFromOptions();
            this.ApplyVisibleLinesFromOptions();
            this.EnsureActiveScrollGroupVisible();
        }

        public void UpdateRuntimeOptions(RuntimeOptions options)
        {
            if (options == null) return;
            this.runtimeOptions = options;
            this.ApplyDefaultGroupVisibilityFromOptions();
            this.ApplyVisibleLinesFromOptions();
            this.EnsureActiveScrollGroupVisible();
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
            if (this.viewModel == null) return this.widgetLines;

            if (this.showCacheGroup)
            {
                this.widgetLines.Add(string.Format("== Cache{0} ({1}/{2}) ==", this.GetFocusMarker("Cache"), this.viewModel.CacheEntries.Count, this.overlay?.RecentCacheEntries?.Count ?? 0));
                this.AppendSummaryLines(this.viewModel.CacheLines);
                this.AppendEntryLines(this.FilterLatestStructuredDuplicate(
                    this.viewModel.CacheEntries,
                    "Cache",
                    this.viewModel.CacheLines != null && this.viewModel.CacheLines.Count > 0));
            }

            if (this.showTransitionGroup)
            {
                this.widgetLines.Add(string.Format("== Transition{0} ({1}/{2}) ==", this.GetFocusMarker("Transition"), this.viewModel.TransitionEntries.Count, this.overlay?.RecentTransitionEntries?.Count ?? 0));
                this.AppendSummaryLines(this.viewModel.TransitionLines);
                this.AppendEntryLines(this.FilterLatestStructuredDuplicate(
                    this.viewModel.TransitionEntries,
                    "Transition",
                    this.viewModel.TransitionLines != null && this.viewModel.TransitionLines.Count > 0));
            }

            if (this.showMiscGroup)
            {
                if (this.viewModel.PersistenceAlertLines != null && this.viewModel.PersistenceAlertLines.Count > 0)
                {
                    this.widgetLines.Add("== PersistenceAlert ==");
                    this.AppendSummaryLines(this.viewModel.PersistenceAlertLines);
                }

                if (this.viewModel.ReloadLines != null && this.viewModel.ReloadLines.Count > 0)
                {
                    this.widgetLines.Add("== Reload ==");
                    this.AppendSummaryLines(this.viewModel.ReloadLines);
                }

                if (this.viewModel.InputLines != null && this.viewModel.InputLines.Count > 0)
                {
                    this.widgetLines.Add("== Input ==");
                    this.AppendSummaryLines(this.viewModel.InputLines);
                }

                this.widgetLines.Add(string.Format("== Misc{0} ({1}/{2}) ==", this.GetFocusMarker("Misc"), this.viewModel.MiscEntries.Count, this.overlay?.RecentMiscEntries?.Count ?? 0));
                var miscEntries = this.FilterLatestStructuredDuplicate(
                    this.viewModel.MiscEntries,
                    "Reload",
                    this.viewModel.ReloadLines != null && this.viewModel.ReloadLines.Count > 0);
                miscEntries = this.FilterLatestStructuredDuplicate(
                    miscEntries,
                    "PersistenceAlert",
                    this.viewModel.PersistenceAlertLines != null && this.viewModel.PersistenceAlertLines.Count > 0);
                miscEntries = this.FilterLatestStructuredDuplicate(
                    miscEntries,
                    "Input",
                    this.viewModel.InputLines != null && this.viewModel.InputLines.Count > 0);
                this.AppendEntryLines(miscEntries);
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

            this.EnsureActiveScrollGroupVisible();
            var scrolled = this.HandleMouseWheelDeltaForGroup(this.activeScrollGroup, wheelDelta);

            if (!scrolled)
            {
                return;
            }

            this.UpdateViewModelMessages();
            this.widgetDirty = true;
        }

        public bool HandleMouseWheelDeltaForGroup(string group, int wheelDelta)
        {
            if (wheelDelta == 0 || this.viewModel == null)
            {
                return false;
            }

            var step = wheelDelta > 0 ? -1 : 1;
            if (string.Equals(group, "Cache", System.StringComparison.OrdinalIgnoreCase))
            {
                if (!this.showCacheGroup) return false;
                this.viewModel.CacheScrollOffset = this.ClampScrollOffset(
                    this.viewModel.CacheScrollOffset + step,
                    this.overlay?.RecentCacheEntries?.Count ?? 0);
                return true;
            }

            if (string.Equals(group, "Transition", System.StringComparison.OrdinalIgnoreCase))
            {
                if (!this.showTransitionGroup) return false;
                this.viewModel.TransitionScrollOffset = this.ClampScrollOffset(
                    this.viewModel.TransitionScrollOffset + step,
                    this.overlay?.RecentTransitionEntries?.Count ?? 0);
                return true;
            }

            if (!this.showMiscGroup) return false;
            this.viewModel.MiscScrollOffset = this.ClampScrollOffset(
                this.viewModel.MiscScrollOffset + step,
                this.overlay?.RecentMiscEntries?.Count ?? 0);
            return true;
        }

        private void UpdateViewModelMessages()
        {
            if (this.viewModel == null)
            {
                this.viewModel = BuildDefaultViewModel(this.styleTokens);
            }

            var maxVisible = this.GetMaxVisibleLinesPerGroup();
            this.viewModel.CacheScrollOffset = this.ClampScrollOffset(
                this.viewModel.CacheScrollOffset,
                this.overlay?.RecentCacheEntries?.Count ?? 0);
            this.viewModel.TransitionScrollOffset = this.ClampScrollOffset(
                this.viewModel.TransitionScrollOffset,
                this.overlay?.RecentTransitionEntries?.Count ?? 0);
            this.viewModel.MiscScrollOffset = this.ClampScrollOffset(
                this.viewModel.MiscScrollOffset,
                this.overlay?.RecentMiscEntries?.Count ?? 0);

            this.viewModel.CacheEntries = this.TakeWindow(
                this.overlay?.RecentCacheEntries?.ToList() ?? new List<ToolBarDateRunnerPolicyDiagnosticsEntry>(),
                this.viewModel.CacheScrollOffset,
                maxVisible);
            this.viewModel.TransitionEntries = this.TakeWindow(
                this.overlay?.RecentTransitionEntries?.ToList() ?? new List<ToolBarDateRunnerPolicyDiagnosticsEntry>(),
                this.viewModel.TransitionScrollOffset,
                maxVisible);
            this.viewModel.MiscEntries = this.TakeWindow(
                this.overlay?.RecentMiscEntries?.ToList() ?? new List<ToolBarDateRunnerPolicyDiagnosticsEntry>(),
                this.viewModel.MiscScrollOffset,
                maxVisible);

            this.viewModel.Entries = this.viewModel.CacheEntries
                .Concat(this.viewModel.TransitionEntries)
                .Concat(this.viewModel.MiscEntries)
                .ToList();
            this.viewModel.Messages = this.viewModel.Entries
                .Select(entry => string.Format("[{0}] {1}", entry.Category, entry.Message))
                .ToList();
            this.viewModel.PersistenceAlertSnapshot = this.overlay?.LatestPersistenceAlertSnapshot;
            this.viewModel.CacheSnapshot = this.overlay?.LatestCacheSnapshot;
            this.viewModel.TransitionSnapshot = this.overlay?.LatestTransitionSnapshot;
            this.viewModel.ReloadSnapshot = this.overlay?.LatestReloadSnapshot;
            this.viewModel.InputSnapshot = this.overlay?.LatestInputSnapshot;
            this.viewModel.PersistenceAlertLines = this.BuildPersistenceAlertLines(this.viewModel.PersistenceAlertSnapshot);
            this.viewModel.CacheLines = this.BuildCacheLines(this.viewModel.CacheSnapshot);
            this.viewModel.TransitionLines = this.BuildTransitionLines(this.viewModel.TransitionSnapshot);
            this.viewModel.ReloadLines = this.BuildReloadLines(this.viewModel.ReloadSnapshot);
            this.viewModel.InputLines = this.BuildInputLines(this.viewModel.InputSnapshot);
            this.widgetDirty = true;
        }

        private void AppendSummaryLines(IReadOnlyCollection<string> lines)
        {
            if (lines == null || lines.Count <= 0) return;
            this.widgetLines.AddRange(lines.Select(message => "• " + message));
        }

        private void AppendEntryLines(IReadOnlyCollection<ToolBarDateRunnerPolicyDiagnosticsEntry> entries)
        {
            if (entries == null || entries.Count <= 0) return;
            this.widgetLines.AddRange(entries.Select(entry => string.Format("• [{0}] {1}", entry.Category, entry.Message)));
        }

        private List<ToolBarDateRunnerPolicyDiagnosticsEntry> FilterLatestStructuredDuplicate(
            IReadOnlyCollection<ToolBarDateRunnerPolicyDiagnosticsEntry> entries,
            string structuredCategory,
            bool hasStructuredSummary)
        {
            var result = entries?.ToList() ?? new List<ToolBarDateRunnerPolicyDiagnosticsEntry>();
            if (!hasStructuredSummary || string.IsNullOrWhiteSpace(structuredCategory))
            {
                return result;
            }

            for (var i = result.Count - 1; i >= 0; i--)
            {
                if (string.Equals(result[i].Category, structuredCategory, System.StringComparison.OrdinalIgnoreCase))
                {
                    result.RemoveAt(i);
                    break;
                }
            }

            return result;
        }

        private List<ToolBarDateRunnerPolicyDiagnosticsEntry> TakeWindow(
            List<ToolBarDateRunnerPolicyDiagnosticsEntry> entries,
            int offset,
            int maxVisible)
        {
            if (entries == null || entries.Count <= 0) return new List<ToolBarDateRunnerPolicyDiagnosticsEntry>();
            if (maxVisible <= 0) maxVisible = 1;
            var skip = offset < 0 ? 0 : offset;
            if (skip > entries.Count) skip = entries.Count;
            return entries.Skip(skip).Take(maxVisible).ToList();
        }

        private int ClampScrollOffset(int offset, int totalCount)
        {
            var maxVisible = this.GetMaxVisibleLinesPerGroup();
            var maxOffset = totalCount > maxVisible ? totalCount - maxVisible : 0;
            if (offset < 0) return 0;
            return offset > maxOffset ? maxOffset : offset;
        }

        private int GetMaxVisibleLinesPerGroup()
        {
            if (this.viewModel == null) return 6;
            if (this.viewModel.MaxVisibleLinesPerGroup > 0) return this.viewModel.MaxVisibleLinesPerGroup;
            return this.viewModel.MaxVisibleLines <= 0 ? 6 : this.viewModel.MaxVisibleLines;
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


        private List<string> BuildCacheLines(ToolBarDateRunnerPolicyCacheDiagnosticsSnapshot snapshot)
        {
            var lines = new List<string>();
            if (snapshot == null) return lines;
            if (snapshot.BeforeFirstEvaluation)
            {
                lines.Add(string.Format("Reason={0}, before first evaluation", snapshot.Reason));
                return lines;
            }

            lines.Add(string.Format(
                "Reason={0}, LastReason={1}, Invalidations={2}",
                snapshot.Reason,
                snapshot.LastInvalidationReason,
                snapshot.InvalidationCount));
            lines.Add(string.Format(
                "Hits={0}, Misses={1}, HitRate={2:F2}%, MissRate={3:F2}%",
                snapshot.CacheHitCount,
                snapshot.CacheMissCount,
                snapshot.HitRate,
                snapshot.MissRate));
            return lines;
        }

        private List<string> BuildTransitionLines(ToolBarDateRunnerPolicyTransitionDiagnosticsSnapshot snapshot)
        {
            var lines = new List<string>();
            if (snapshot == null) return lines;
            if (snapshot.InitialSnapshot)
            {
                lines.Add(string.Format("Initial snapshot observed. Reason={0}", snapshot.Reason));
                return lines;
            }

            lines.Add(string.Format(
                "Reason={0}, Changed(Suspend/Lock/Flow)={1}/{2}/{3}",
                snapshot.Reason,
                snapshot.SuspendDateRunnerChanged,
                snapshot.LockToolBarInputChanged,
                snapshot.FlowPolicyChanged));
            lines.Add(string.Format(
                "Suspend={0}->{1}, Lock={2}->{3}",
                snapshot.PreviousSnapshot.Decision.SuspendDateRunner,
                snapshot.CurrentSnapshot.Decision.SuspendDateRunner,
                snapshot.PreviousSnapshot.Decision.LockToolBarInput,
                snapshot.CurrentSnapshot.Decision.LockToolBarInput));
            return lines;
        }

        private List<string> BuildReloadLines(ToolBarDateRunnerPolicyReloadDiagnosticsSnapshot snapshot)
        {
            var lines = new List<string>();
            if (snapshot == null) return lines;
            lines.Add(string.Format(
                "Source={0}, DebugOverlay={1}",
                snapshot.Source,
                snapshot.DebugOverlayEnabled ? "ON" : "OFF"));
            return lines;
        }



        private List<string> BuildInputLines(ToolBarDateRunnerPolicyInputDiagnosticsSnapshot snapshot)
        {
            var lines = new List<string>();
            if (snapshot == null) return lines;
            lines.Add(string.Format(
                "Action={0}, Key={1}, Result={2}",
                snapshot.Action,
                snapshot.Key,
                snapshot.Result));
            if (!string.IsNullOrWhiteSpace(snapshot.Detail))
            {
                lines.Add(snapshot.Detail);
            }

            return lines;
        }

        private string GetFocusMarker(string group)
        {
            return string.Equals(this.activeScrollGroup, group, System.StringComparison.OrdinalIgnoreCase) ? " *focus*" : string.Empty;
        }

        private void EnsureActiveScrollGroupVisible()
        {
            if (this.IsGroupVisible(this.activeScrollGroup))
            {
                return;
            }

            if (this.showCacheGroup)
            {
                this.activeScrollGroup = "Cache";
                return;
            }

            if (this.showTransitionGroup)
            {
                this.activeScrollGroup = "Transition";
                return;
            }

            if (this.showMiscGroup)
            {
                this.activeScrollGroup = "Misc";
                return;
            }

            this.showCacheGroup = true;
            this.activeScrollGroup = "Cache";
        }

        private bool IsGroupVisible(string group)
        {
            if (string.Equals(group, "Cache", System.StringComparison.OrdinalIgnoreCase)) return this.showCacheGroup;
            if (string.Equals(group, "Transition", System.StringComparison.OrdinalIgnoreCase)) return this.showTransitionGroup;
            if (string.Equals(group, "Misc", System.StringComparison.OrdinalIgnoreCase)) return this.showMiscGroup;
            return false;
        }

        private void ApplyVisibleLinesFromOptions()
        {
            if (this.viewModel == null) return;
            var configured = this.runtimeOptions?.Ui?.ToolBarDateRunnerPolicy?.DiagnosticsOverlayMaxVisibleLinesPerGroup ?? 6;
            this.viewModel.MaxVisibleLinesPerGroup = configured <= 0 ? 6 : configured;
            this.viewModel.CacheScrollOffset = this.ClampScrollOffset(this.viewModel.CacheScrollOffset, this.overlay?.RecentCacheEntries?.Count ?? 0);
            this.viewModel.TransitionScrollOffset = this.ClampScrollOffset(this.viewModel.TransitionScrollOffset, this.overlay?.RecentTransitionEntries?.Count ?? 0);
            this.viewModel.MiscScrollOffset = this.ClampScrollOffset(this.viewModel.MiscScrollOffset, this.overlay?.RecentMiscEntries?.Count ?? 0);
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
                this.activeScrollGroup = "Cache";
            }
            else if (this.showCacheGroup && !this.showTransitionGroup)
            {
                this.showCacheGroup = false;
                this.showTransitionGroup = true;
                this.activeScrollGroup = "Transition";
            }
            else
            {
                this.showCacheGroup = true;
                this.showTransitionGroup = true;
                this.showMiscGroup = true;
                this.activeScrollGroup = "Misc";
            }

            this.EnsureActiveScrollGroupVisible();
            this.widgetDirty = true;
            return this.GetGroupModeDescription();
        }


        public string CycleActiveScrollGroup()
        {
            if (string.Equals(this.activeScrollGroup, "Cache", System.StringComparison.OrdinalIgnoreCase))
            {
                if (this.showTransitionGroup)
                {
                    this.activeScrollGroup = "Transition";
                }
                else if (this.showMiscGroup)
                {
                    this.activeScrollGroup = "Misc";
                }
            }
            else if (string.Equals(this.activeScrollGroup, "Transition", System.StringComparison.OrdinalIgnoreCase))
            {
                if (this.showMiscGroup)
                {
                    this.activeScrollGroup = "Misc";
                }
                else if (this.showCacheGroup)
                {
                    this.activeScrollGroup = "Cache";
                }
            }
            else
            {
                if (this.showCacheGroup)
                {
                    this.activeScrollGroup = "Cache";
                }
                else if (this.showTransitionGroup)
                {
                    this.activeScrollGroup = "Transition";
                }
            }

            this.EnsureActiveScrollGroupVisible();
            this.widgetDirty = true;
            return this.GetGroupModeDescription();
        }

        public string GetGroupModeDescription()
        {
            return string.Format(
                "Groups: Cache={0}, Transition={1}, Misc={2}, Focus={3}, LinesPerGroup={4}",
                this.showCacheGroup ? "ON" : "OFF",
                this.showTransitionGroup ? "ON" : "OFF",
                this.showMiscGroup ? "ON" : "OFF",
                this.activeScrollGroup,
                this.viewModel?.MaxVisibleLinesPerGroup ?? 6);
        }

        private void ApplyDefaultGroupVisibilityFromOptions()
        {
            var policy = this.runtimeOptions?.Ui?.ToolBarDateRunnerPolicy;
            if (policy == null) return;
            this.showCacheGroup = policy.DiagnosticsOverlayShowCacheGroupByDefault;
            this.showTransitionGroup = policy.DiagnosticsOverlayShowTransitionGroupByDefault;
            this.showMiscGroup = policy.DiagnosticsOverlayShowMiscGroupByDefault;
            this.ApplyPreset(policy.DiagnosticsOverlayPreset);
            this.ApplyVisibleLinesFromOptions();
            this.EnsureActiveScrollGroupVisible();
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

            this.EnsureActiveScrollGroupVisible();
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
                this.activeScrollGroup = "Cache";
                return;
            }

            if (string.Equals(preset, "Minimal", System.StringComparison.OrdinalIgnoreCase))
            {
                this.showCacheGroup = false;
                this.showTransitionGroup = true;
                this.showMiscGroup = false;
                this.currentPreset = "Minimal";
                this.activeScrollGroup = "Transition";
                return;
            }

            this.showCacheGroup = true;
            this.showTransitionGroup = true;
            this.showMiscGroup = true;
            this.currentPreset = "Debug";
            this.activeScrollGroup = "Cache";
        }
    }
}
