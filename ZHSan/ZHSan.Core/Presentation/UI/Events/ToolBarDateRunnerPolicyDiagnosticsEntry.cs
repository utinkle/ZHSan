namespace ZHSan.Core.Presentation.UI.Events
{
    public sealed class ToolBarDateRunnerPolicyDiagnosticsEntry
    {
        public ToolBarDateRunnerPolicyDiagnosticsEntry(string category, string reason, string message)
        {
            this.Category = string.IsNullOrWhiteSpace(category) ? "Unknown" : category;
            this.Reason = string.IsNullOrWhiteSpace(reason) ? "unspecified" : reason;
            this.Message = message ?? string.Empty;
        }

        public string Category { get; }
        public string Reason { get; }
        public string Message { get; }
    }
}
