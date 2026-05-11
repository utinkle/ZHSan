using ZHSan.Core.Presentation.UI.Services;

namespace ZHSan.Core.Presentation.UI.Events
{
    public sealed class ToolBarDateRunnerPolicyDiagnosticsEvent
    {
        public ToolBarDateRunnerPolicyDiagnosticsEvent(string category, string reason, ToolBarDateRunnerPolicySnapshot snapshot, string message)
        {
            this.Category = category;
            this.Reason = reason;
            this.Snapshot = snapshot;
            this.Message = message;
        }

        public string Category { get; }
        public string Reason { get; }
        public ToolBarDateRunnerPolicySnapshot Snapshot { get; }
        public string Message { get; }
    }
}
