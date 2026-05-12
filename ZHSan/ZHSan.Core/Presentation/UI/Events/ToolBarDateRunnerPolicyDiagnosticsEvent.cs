using ZHSan.Core.Presentation.UI.Services;

namespace ZHSan.Core.Presentation.UI.Events
{
    public sealed class ToolBarDateRunnerPolicyDiagnosticsEvent
    {
        public ToolBarDateRunnerPolicyDiagnosticsEvent(string category, string reason, ToolBarDateRunnerPolicySnapshot snapshot, string message)
            : this(category, reason, snapshot, message, null)
        {
        }

        public ToolBarDateRunnerPolicyDiagnosticsEvent(
            string category,
            string reason,
            ToolBarDateRunnerPolicySnapshot snapshot,
            string message,
            RuntimeOptionsPersistenceAlertSnapshot persistenceAlertSnapshot)
            : this(category, reason, snapshot, message, persistenceAlertSnapshot, null, null, null, null)
        {
        }

        public ToolBarDateRunnerPolicyDiagnosticsEvent(
            string category,
            string reason,
            ToolBarDateRunnerPolicySnapshot snapshot,
            string message,
            RuntimeOptionsPersistenceAlertSnapshot persistenceAlertSnapshot,
            ToolBarDateRunnerPolicyCacheDiagnosticsSnapshot cacheSnapshot,
            ToolBarDateRunnerPolicyTransitionDiagnosticsSnapshot transitionSnapshot,
            ToolBarDateRunnerPolicyReloadDiagnosticsSnapshot reloadSnapshot)
            : this(category, reason, snapshot, message, persistenceAlertSnapshot, cacheSnapshot, transitionSnapshot, reloadSnapshot, null)
        {
        }

        public ToolBarDateRunnerPolicyDiagnosticsEvent(
            string category,
            string reason,
            ToolBarDateRunnerPolicySnapshot snapshot,
            string message,
            RuntimeOptionsPersistenceAlertSnapshot persistenceAlertSnapshot,
            ToolBarDateRunnerPolicyCacheDiagnosticsSnapshot cacheSnapshot,
            ToolBarDateRunnerPolicyTransitionDiagnosticsSnapshot transitionSnapshot,
            ToolBarDateRunnerPolicyReloadDiagnosticsSnapshot reloadSnapshot,
            ToolBarDateRunnerPolicyInputDiagnosticsSnapshot inputSnapshot)
        {
            this.Category = category;
            this.Reason = reason;
            this.Snapshot = snapshot;
            this.Message = message;
            this.PersistenceAlertSnapshot = persistenceAlertSnapshot;
            this.CacheSnapshot = cacheSnapshot;
            this.TransitionSnapshot = transitionSnapshot;
            this.ReloadSnapshot = reloadSnapshot;
            this.InputSnapshot = inputSnapshot;
        }

        public string Category { get; }
        public string Reason { get; }
        public ToolBarDateRunnerPolicySnapshot Snapshot { get; }
        public string Message { get; }
        public RuntimeOptionsPersistenceAlertSnapshot PersistenceAlertSnapshot { get; }
        public ToolBarDateRunnerPolicyCacheDiagnosticsSnapshot CacheSnapshot { get; }
        public ToolBarDateRunnerPolicyTransitionDiagnosticsSnapshot TransitionSnapshot { get; }
        public ToolBarDateRunnerPolicyReloadDiagnosticsSnapshot ReloadSnapshot { get; }
        public ToolBarDateRunnerPolicyInputDiagnosticsSnapshot InputSnapshot { get; }
    }
}
