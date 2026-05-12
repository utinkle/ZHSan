namespace ZHSan.Core.Presentation.UI.Events
{
    public sealed class ToolBarDateRunnerPolicyInputDiagnosticsSnapshot
    {
        public ToolBarDateRunnerPolicyInputDiagnosticsSnapshot(string action, string key, string result, string detail)
        {
            this.Action = string.IsNullOrWhiteSpace(action) ? "Unknown" : action;
            this.Key = string.IsNullOrWhiteSpace(key) ? "N/A" : key;
            this.Result = string.IsNullOrWhiteSpace(result) ? "N/A" : result;
            this.Detail = detail ?? string.Empty;
        }

        public string Action { get; }
        public string Key { get; }
        public string Result { get; }
        public string Detail { get; }
    }
}
