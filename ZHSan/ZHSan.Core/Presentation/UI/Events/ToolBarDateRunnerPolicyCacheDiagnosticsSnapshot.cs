namespace ZHSan.Core.Presentation.UI.Events
{
    public sealed class ToolBarDateRunnerPolicyCacheDiagnosticsSnapshot
    {
        public ToolBarDateRunnerPolicyCacheDiagnosticsSnapshot(
            string reason,
            string lastInvalidationReason,
            int invalidationCount,
            int cacheHitCount,
            int cacheMissCount,
            double hitRate,
            double missRate,
            bool beforeFirstEvaluation)
        {
            this.Reason = reason ?? "unspecified";
            this.LastInvalidationReason = lastInvalidationReason ?? "unspecified";
            this.InvalidationCount = invalidationCount;
            this.CacheHitCount = cacheHitCount;
            this.CacheMissCount = cacheMissCount;
            this.HitRate = hitRate;
            this.MissRate = missRate;
            this.BeforeFirstEvaluation = beforeFirstEvaluation;
        }

        public string Reason { get; }
        public string LastInvalidationReason { get; }
        public int InvalidationCount { get; }
        public int CacheHitCount { get; }
        public int CacheMissCount { get; }
        public double HitRate { get; }
        public double MissRate { get; }
        public bool BeforeFirstEvaluation { get; }
    }
}
