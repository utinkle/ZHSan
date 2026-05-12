using System;
using System.Collections.Generic;
using ZHSan.Core.Application.Events;
using ZHSan.Core.Infrastructure.Configuration;
using ZHSan.Core.Presentation.UI.Events;

namespace ZHSan.Core.Presentation.UI.Services
{
    /// <summary>
    /// Debug overlay sample subscriber for ToolBarDateRunnerPolicy diagnostics events.
    /// This service is intentionally UI-framework-agnostic; concrete overlay widgets can bind to structured snapshots and RecentMessages.
    /// </summary>
    public sealed class ToolBarDateRunnerPolicyDebugOverlay : IDisposable, IRuntimeOptionsReloadHandler
    {
        private readonly Queue<string> recentMessages = new Queue<string>();
        private readonly Queue<ToolBarDateRunnerPolicyDiagnosticsEntry> recentEntries = new Queue<ToolBarDateRunnerPolicyDiagnosticsEntry>();
        private readonly Queue<ToolBarDateRunnerPolicyDiagnosticsEntry> recentCacheEntries = new Queue<ToolBarDateRunnerPolicyDiagnosticsEntry>();
        private readonly Queue<ToolBarDateRunnerPolicyDiagnosticsEntry> recentTransitionEntries = new Queue<ToolBarDateRunnerPolicyDiagnosticsEntry>();
        private readonly Queue<ToolBarDateRunnerPolicyDiagnosticsEntry> recentMiscEntries = new Queue<ToolBarDateRunnerPolicyDiagnosticsEntry>();
        private readonly IDisposable subscription;
        private int maxMessages;
        private string minimumCategory;
        private RuntimeOptionsPersistenceAlertSnapshot latestPersistenceAlertSnapshot;
        private ToolBarDateRunnerPolicyCacheDiagnosticsSnapshot latestCacheSnapshot;
        private ToolBarDateRunnerPolicyTransitionDiagnosticsSnapshot latestTransitionSnapshot;
        private ToolBarDateRunnerPolicyReloadDiagnosticsSnapshot latestReloadSnapshot;
        private ToolBarDateRunnerPolicyInputDiagnosticsSnapshot latestInputSnapshot;

        public ToolBarDateRunnerPolicyDebugOverlay(IEventBus eventBus, RuntimeOptions options)
        {
            this.ApplyRuntimeOptions(options, "Bootstrap");
            if (eventBus != null)
            {
                this.subscription = eventBus.Subscribe<ToolBarDateRunnerPolicyDiagnosticsEvent>(this.OnDiagnosticsEvent);
            }
        }

        public IReadOnlyCollection<string> RecentMessages => this.recentMessages.ToArray();

        public IReadOnlyCollection<ToolBarDateRunnerPolicyDiagnosticsEntry> RecentEntries => this.recentEntries.ToArray();

        public IReadOnlyCollection<ToolBarDateRunnerPolicyDiagnosticsEntry> RecentCacheEntries => this.recentCacheEntries.ToArray();

        public IReadOnlyCollection<ToolBarDateRunnerPolicyDiagnosticsEntry> RecentTransitionEntries => this.recentTransitionEntries.ToArray();

        public IReadOnlyCollection<ToolBarDateRunnerPolicyDiagnosticsEntry> RecentMiscEntries => this.recentMiscEntries.ToArray();

        public int MaxMessages => this.maxMessages;

        public RuntimeOptionsPersistenceAlertSnapshot LatestPersistenceAlertSnapshot => this.latestPersistenceAlertSnapshot;

        public ToolBarDateRunnerPolicyCacheDiagnosticsSnapshot LatestCacheSnapshot => this.latestCacheSnapshot;

        public ToolBarDateRunnerPolicyTransitionDiagnosticsSnapshot LatestTransitionSnapshot => this.latestTransitionSnapshot;

        public ToolBarDateRunnerPolicyReloadDiagnosticsSnapshot LatestReloadSnapshot => this.latestReloadSnapshot;

        public ToolBarDateRunnerPolicyInputDiagnosticsSnapshot LatestInputSnapshot => this.latestInputSnapshot;

        private void OnDiagnosticsEvent(ToolBarDateRunnerPolicyDiagnosticsEvent evt)
        {
            if (evt == null) return;
            this.UpdateStructuredSnapshots(evt);
            if (!this.ShouldAccept(evt.Category)) return;
            var entry = new ToolBarDateRunnerPolicyDiagnosticsEntry(evt.Category, evt.Reason, evt.Message);
            var line = string.Format("[{0}] {1}", entry.Category, entry.Message);
            this.recentEntries.Enqueue(entry);
            this.EnqueueCategoryEntry(entry);
            this.recentMessages.Enqueue(line);
            this.TrimQueues();
        }

        public void Dispose()
        {
            this.subscription?.Dispose();
        }

        public void ApplyRuntimeOptions(RuntimeOptions options, string source)
        {
            var policy = options?.Ui?.ToolBarDateRunnerPolicy;
            this.maxMessages = Math.Max(1, policy?.DiagnosticsOverlayMaxMessages ?? 30);
            this.minimumCategory = string.IsNullOrWhiteSpace(policy?.DiagnosticsOverlayMinimumCategory)
                ? "Cache"
                : policy.DiagnosticsOverlayMinimumCategory.Trim();
            this.TrimQueues();
        }


        private void EnqueueCategoryEntry(ToolBarDateRunnerPolicyDiagnosticsEntry entry)
        {
            if (entry == null) return;
            if (string.Equals(entry.Category, "Cache", StringComparison.OrdinalIgnoreCase))
            {
                this.recentCacheEntries.Enqueue(entry);
                return;
            }

            if (string.Equals(entry.Category, "Transition", StringComparison.OrdinalIgnoreCase))
            {
                this.recentTransitionEntries.Enqueue(entry);
                return;
            }

            this.recentMiscEntries.Enqueue(entry);
        }

        private void TrimQueues()
        {
            this.TrimQueue(this.recentEntries);
            this.TrimQueue(this.recentCacheEntries);
            this.TrimQueue(this.recentTransitionEntries);
            this.TrimQueue(this.recentMiscEntries);
            while (this.recentMessages.Count > this.maxMessages)
            {
                this.recentMessages.Dequeue();
            }
        }

        private void TrimQueue(Queue<ToolBarDateRunnerPolicyDiagnosticsEntry> queue)
        {
            if (queue == null) return;
            while (queue.Count > this.maxMessages)
            {
                queue.Dequeue();
            }
        }

        private void UpdateStructuredSnapshots(ToolBarDateRunnerPolicyDiagnosticsEvent evt)
        {
            if (evt.PersistenceAlertSnapshot != null)
            {
                this.latestPersistenceAlertSnapshot = evt.PersistenceAlertSnapshot;
            }

            if (evt.CacheSnapshot != null)
            {
                this.latestCacheSnapshot = evt.CacheSnapshot;
            }

            if (evt.TransitionSnapshot != null)
            {
                this.latestTransitionSnapshot = evt.TransitionSnapshot;
            }

            if (evt.ReloadSnapshot != null)
            {
                this.latestReloadSnapshot = evt.ReloadSnapshot;
            }

            if (evt.InputSnapshot != null)
            {
                this.latestInputSnapshot = evt.InputSnapshot;
            }
        }

        private bool ShouldAccept(string category)
        {
            if (string.Equals(this.minimumCategory, "cache", StringComparison.OrdinalIgnoreCase))
            {
                return true;
            }

            if (string.Equals(this.minimumCategory, "transition", StringComparison.OrdinalIgnoreCase))
            {
                return string.Equals(category, "transition", StringComparison.OrdinalIgnoreCase);
            }

            return true;
        }
    }
}
