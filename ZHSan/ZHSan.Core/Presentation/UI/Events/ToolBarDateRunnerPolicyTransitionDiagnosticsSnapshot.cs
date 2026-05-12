using ZHSan.Core.Presentation.UI.Services;

namespace ZHSan.Core.Presentation.UI.Events
{
    public sealed class ToolBarDateRunnerPolicyTransitionDiagnosticsSnapshot
    {
        public ToolBarDateRunnerPolicyTransitionDiagnosticsSnapshot(
            string reason,
            bool initialSnapshot,
            ToolBarDateRunnerPolicySnapshot previousSnapshot,
            ToolBarDateRunnerPolicySnapshot currentSnapshot,
            bool suspendDateRunnerChanged,
            bool lockToolBarInputChanged,
            bool flowPolicyChanged)
        {
            this.Reason = reason ?? "unspecified";
            this.InitialSnapshot = initialSnapshot;
            this.PreviousSnapshot = previousSnapshot;
            this.CurrentSnapshot = currentSnapshot;
            this.SuspendDateRunnerChanged = suspendDateRunnerChanged;
            this.LockToolBarInputChanged = lockToolBarInputChanged;
            this.FlowPolicyChanged = flowPolicyChanged;
        }

        public string Reason { get; }
        public bool InitialSnapshot { get; }
        public ToolBarDateRunnerPolicySnapshot PreviousSnapshot { get; }
        public ToolBarDateRunnerPolicySnapshot CurrentSnapshot { get; }
        public bool SuspendDateRunnerChanged { get; }
        public bool LockToolBarInputChanged { get; }
        public bool FlowPolicyChanged { get; }
    }
}
