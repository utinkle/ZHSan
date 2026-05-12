namespace ZHSan.Core.Presentation.UI.Events
{
    public sealed class ToolBarDateRunnerPolicyInputDiagnosticsSnapshot
    {
        public ToolBarDateRunnerPolicyInputDiagnosticsSnapshot(
            string action,
            string key,
            string result,
            string detail,
            int suppressedCount,
            int dedupWindowMs,
            bool dedupEnabled,
            double elapsedSinceLastPublishedMs,
            int trendWindowSeconds,
            int trendSuppressedCount,
            int trendPublishedCount,
            bool isFirstSampleAfterTrendWindowReload,
            int reloadSummarySkippedCount)
        {
            this.Action = string.IsNullOrWhiteSpace(action) ? "Unknown" : action;
            this.Key = string.IsNullOrWhiteSpace(key) ? "N/A" : key;
            this.Result = string.IsNullOrWhiteSpace(result) ? "N/A" : result;
            this.Detail = detail ?? string.Empty;
            this.SuppressedCount = suppressedCount < 0 ? 0 : suppressedCount;
            this.DedupWindowMs = dedupWindowMs < 0 ? 0 : dedupWindowMs;
            this.DedupEnabled = dedupEnabled;
            this.ElapsedSinceLastPublishedMs = elapsedSinceLastPublishedMs < 0 ? 0 : elapsedSinceLastPublishedMs;
            this.TrendWindowSeconds = trendWindowSeconds < 1 ? 1 : trendWindowSeconds;
            this.TrendSuppressedCount = trendSuppressedCount < 0 ? 0 : trendSuppressedCount;
            this.TrendPublishedCount = trendPublishedCount < 0 ? 0 : trendPublishedCount;
            this.IsFirstSampleAfterTrendWindowReload = isFirstSampleAfterTrendWindowReload;
            this.ReloadSummarySkippedCount = reloadSummarySkippedCount < 0 ? 0 : reloadSummarySkippedCount;
        }

        public string Action { get; }
        public string Key { get; }
        public string Result { get; }
        public string Detail { get; }
        public int SuppressedCount { get; }
        public int DedupWindowMs { get; }
        public bool DedupEnabled { get; }
        public double ElapsedSinceLastPublishedMs { get; }
        public int TrendWindowSeconds { get; }
        public int TrendSuppressedCount { get; }
        public int TrendPublishedCount { get; }
        public bool IsFirstSampleAfterTrendWindowReload { get; }
        public int ReloadSummarySkippedCount { get; }
        public double TrendSuppressionRate =>
            this.TrendSuppressedCount + this.TrendPublishedCount <= 0
                ? 0
                : (double)this.TrendSuppressedCount / (this.TrendSuppressedCount + this.TrendPublishedCount);
    }
}
