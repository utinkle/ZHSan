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
        {
            this.Category = category;
            this.Reason = reason;
            this.Snapshot = snapshot;
            this.Message = message;
            this.PersistenceAlertSnapshot = persistenceAlertSnapshot;
        }

        public string Category { get; }
        public string Reason { get; }
        public ToolBarDateRunnerPolicySnapshot Snapshot { get; }
        public string Message { get; }
        public RuntimeOptionsPersistenceAlertSnapshot PersistenceAlertSnapshot { get; }
    }
}
