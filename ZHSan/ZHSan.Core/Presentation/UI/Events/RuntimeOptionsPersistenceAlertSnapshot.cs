namespace ZHSan.Core.Presentation.UI.Events
{
    /// <summary>
    /// Structured diagnostics snapshot for RuntimeOptions persistence-alert state.
    /// </summary>
    public sealed class RuntimeOptionsPersistenceAlertSnapshot
    {
        public RuntimeOptionsPersistenceAlertSnapshot(
            string level,
            string trigger,
            int consecutiveFailureCount,
            int threshold,
            int quietWindowMs,
            int thresholdHitCount,
            int emittedAlertCount,
            int quietWindowSuppressedCount,
            int attempt,
            int maxAttempts,
            string path,
            string errorType,
            string message)
        {
            this.Level = string.IsNullOrWhiteSpace(level) ? "Unknown" : level;
            this.Trigger = string.IsNullOrWhiteSpace(trigger) ? "unknown" : trigger;
            this.ConsecutiveFailureCount = consecutiveFailureCount;
            this.Threshold = threshold;
            this.QuietWindowMs = quietWindowMs;
            this.ThresholdHitCount = thresholdHitCount;
            this.EmittedAlertCount = emittedAlertCount;
            this.QuietWindowSuppressedCount = quietWindowSuppressedCount;
            this.Attempt = attempt;
            this.MaxAttempts = maxAttempts;
            this.Path = string.IsNullOrWhiteSpace(path) ? "n/a" : path;
            this.ErrorType = string.IsNullOrWhiteSpace(errorType) ? "n/a" : errorType;
            this.Message = message ?? string.Empty;
        }

        public string Level { get; }

        public string Trigger { get; }

        public int ConsecutiveFailureCount { get; }

        public int Threshold { get; }

        public int QuietWindowMs { get; }

        public int ThresholdHitCount { get; }

        public int EmittedAlertCount { get; }

        public int QuietWindowSuppressedCount { get; }

        public int Attempt { get; }

        public int MaxAttempts { get; }

        public string Path { get; }

        public string ErrorType { get; }

        public string Message { get; }
    }
}
