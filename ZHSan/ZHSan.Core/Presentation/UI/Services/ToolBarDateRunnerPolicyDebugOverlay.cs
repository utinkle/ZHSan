using System;
using System.Collections.Generic;
using ZHSan.Core.Application.Events;
using ZHSan.Core.Infrastructure.Configuration;
using ZHSan.Core.Presentation.UI.Events;

namespace ZHSan.Core.Presentation.UI.Services
{
    /// <summary>
    /// Debug overlay sample subscriber for ToolBarDateRunnerPolicy diagnostics events.
    /// This service is intentionally UI-framework-agnostic; concrete overlay widgets can bind to RecentMessages.
    /// </summary>
    public sealed class ToolBarDateRunnerPolicyDebugOverlay : IDisposable
    {
        private readonly Queue<string> recentMessages = new Queue<string>();
        private readonly IDisposable subscription;
        private readonly int maxMessages;
        private readonly string minimumCategory;

        public ToolBarDateRunnerPolicyDebugOverlay(IEventBus eventBus, RuntimeOptions options)
        {
            var policy = options?.Ui?.ToolBarDateRunnerPolicy;
            this.maxMessages = Math.Max(1, policy?.DiagnosticsOverlayMaxMessages ?? 30);
            this.minimumCategory = string.IsNullOrWhiteSpace(policy?.DiagnosticsOverlayMinimumCategory)
                ? "Cache"
                : policy.DiagnosticsOverlayMinimumCategory.Trim();
            if (eventBus != null)
            {
                this.subscription = eventBus.Subscribe<ToolBarDateRunnerPolicyDiagnosticsEvent>(this.OnDiagnosticsEvent);
            }
        }

        public IReadOnlyCollection<string> RecentMessages => this.recentMessages.ToArray();

        private void OnDiagnosticsEvent(ToolBarDateRunnerPolicyDiagnosticsEvent evt)
        {
            if (evt == null) return;
            if (!this.ShouldAccept(evt.Category)) return;
            var line = string.Format("[{0}] {1}", evt.Category ?? "Unknown", evt.Message ?? string.Empty);
            this.recentMessages.Enqueue(line);
            while (this.recentMessages.Count > this.maxMessages)
            {
                this.recentMessages.Dequeue();
            }
        }

        public void Dispose()
        {
            this.subscription?.Dispose();
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
