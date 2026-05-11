using ZHSan.Core.Infrastructure.Configuration;
using ZHSan.Core.Infrastructure.FeatureFlags;
using WorldOfTheThreeKingdoms.GameScreens;
using System;

namespace ZHSan.Core.Presentation.UI.Services
{
    public sealed class ToolBarDateRunnerPolicyCoordinator : IRuntimeOptionsReloadHandler
    {
        private readonly ToolBarDateRunnerInteractionService interactionService;
        private readonly ToolBarDateRunnerPolicyInputBuilder inputBuilder;
        private readonly ToolBarDateRunnerFlowPolicyBuilder flowPolicyBuilder;
        private bool hasCache;
        private ToolBarDateRunnerPolicyInput lastInput;
        private ToolBarDateRunnerFlowPolicy lastFlowPolicy;
        private ToolBarDateRunnerPolicySnapshot lastSnapshot;
        private bool hasLoggedTransitionSnapshot;
        private ToolBarDateRunnerPolicySnapshot lastTransitionSnapshot;
        private int invalidationCount;
        private string lastInvalidationReason;
        private DateTime lastDiagnosticsLoggedAtUtc = DateTime.MinValue;
        public int CacheHitCount { get; private set; }
        public int CacheMissCount { get; private set; }

        public ToolBarDateRunnerPolicyCoordinator(ToolBarDateRunnerInteractionService interactionService, ToolBarDateRunnerPolicyInputBuilder inputBuilder, ToolBarDateRunnerFlowPolicyBuilder flowPolicyBuilder)
        {
            this.interactionService = interactionService;
            this.inputBuilder = inputBuilder;
            this.flowPolicyBuilder = flowPolicyBuilder;
        }

        public ToolBarDateRunnerPolicySnapshot Evaluate(MainGameScreen screen, RuntimeOptions options, FeatureFlags featureFlags)
        {
            if (featureFlags == null || !featureFlags.UseToolBarDateRunnerPolicy || this.interactionService == null)
            {
                return default;
            }

            var input = this.inputBuilder == null ? default : this.inputBuilder.Build(screen);
            var flowPolicy = this.flowPolicyBuilder == null ? new ToolBarDateRunnerFlowPolicy(true, true, true, true) : this.flowPolicyBuilder.Build(options);

            if (this.hasCache && this.lastInput.Equals(input) && this.lastFlowPolicy.Equals(flowPolicy))
            {
                this.CacheHitCount++;
                return this.lastSnapshot;
            }

            this.CacheMissCount++;
            var decision = this.interactionService.Evaluate(input);
            var snapshot = new ToolBarDateRunnerPolicySnapshot(decision, flowPolicy);
            this.lastInput = input;
            this.lastFlowPolicy = flowPolicy;
            this.lastSnapshot = snapshot;
            this.hasCache = true;
            return snapshot;
        }

        public int InvalidationCount => this.invalidationCount;

        public string LastInvalidationReason => this.lastInvalidationReason;

        public void ApplyRuntimeOptions(RuntimeOptions options, string source)
        {
            this.InvalidateCache(string.Format(
                "RuntimeOptions reloaded from {0}",
                string.IsNullOrWhiteSpace(source) ? "unknown" : source));
        }

        public void InvalidateCache(string reason = null)
        {
            this.invalidationCount++;
            this.lastInvalidationReason = string.IsNullOrWhiteSpace(reason) ? "unspecified" : reason;
            this.hasCache = false;
        }

        public bool ShouldLogCacheDiagnostics(RuntimeOptions options)
        {
            var policyOptions = options?.Ui?.ToolBarDateRunnerPolicy;
            if (policyOptions == null || !policyOptions.EnableCacheDiagnosticsLog)
            {
                return false;
            }

            if (policyOptions.CacheDiagnosticsLogSampleEveryInvalidations > 1
                && this.invalidationCount % policyOptions.CacheDiagnosticsLogSampleEveryInvalidations != 0)
            {
                return false;
            }

            if (policyOptions.CacheDiagnosticsLogMinIntervalMs > 0)
            {
                var nowUtc = DateTime.UtcNow;
                var elapsedMs = (nowUtc - this.lastDiagnosticsLoggedAtUtc).TotalMilliseconds;
                if (elapsedMs < policyOptions.CacheDiagnosticsLogMinIntervalMs)
                {
                    return false;
                }

                this.lastDiagnosticsLoggedAtUtc = nowUtc;
            }

            return true;
        }

        public string BuildCacheDiagnostics(string reason)
        {
            var total = this.CacheHitCount + this.CacheMissCount;
            if (total <= 0)
            {
                return "[ToolBarDateRunnerPolicy] Cache invalidated before first evaluation.";
            }

            var hitRate = (this.CacheHitCount * 100.0) / total;
            var missRate = (this.CacheMissCount * 100.0) / total;
            return string.Format(
                "[ToolBarDateRunnerPolicy] Cache invalidated ({0}). LastReason={1}, Invalidations={2}, Hits={3}, Misses={4}, HitRate={5:F2}%, MissRate={6:F2}%.",
                reason ?? "unspecified",
                this.lastInvalidationReason ?? "unspecified",
                this.invalidationCount,
                this.CacheHitCount,
                this.CacheMissCount,
                hitRate,
                missRate);
        }

        public string TryBuildTransitionDiagnostics(ToolBarDateRunnerPolicySnapshot currentSnapshot, RuntimeOptions options, string reason)
        {
            if (options?.Ui?.ToolBarDateRunnerPolicy == null
                || !options.Ui.ToolBarDateRunnerPolicy.EnablePolicyTransitionDebugLog)
            {
                return null;
            }

            if (!this.hasLoggedTransitionSnapshot)
            {
                this.lastTransitionSnapshot = currentSnapshot;
                this.hasLoggedTransitionSnapshot = true;
                return string.Format("[ToolBarDateRunnerPolicy][Transition] Initial snapshot observed ({0}).", reason ?? "unspecified");
            }

            if (this.lastTransitionSnapshot.Equals(currentSnapshot))
            {
                return null;
            }

            var previous = this.lastTransitionSnapshot;
            this.lastTransitionSnapshot = currentSnapshot;
            return string.Format(
                "[ToolBarDateRunnerPolicy][Transition] {0}: SuspendDateRunner {1} -> {2}, LockToolBarInput {3} -> {4}, FlowPolicy(None/Selector/Map/Dialog) {5}/{6}/{7}/{8} -> {9}/{10}/{11}/{12}.",
                reason ?? "state changed",
                previous.Decision.SuspendDateRunner,
                currentSnapshot.Decision.SuspendDateRunner,
                previous.Decision.LockToolBarInput,
                currentSnapshot.Decision.LockToolBarInput,
                previous.FlowPolicy.LockInNoneFlow,
                previous.FlowPolicy.LockInSelector,
                previous.FlowPolicy.LockInMapViewSelector,
                previous.FlowPolicy.LockInDialog,
                currentSnapshot.FlowPolicy.LockInNoneFlow,
                currentSnapshot.FlowPolicy.LockInSelector,
                currentSnapshot.FlowPolicy.LockInMapViewSelector,
                currentSnapshot.FlowPolicy.LockInDialog);
        }
    }

    public readonly struct ToolBarDateRunnerPolicySnapshot
    {
        public ToolBarDateRunnerPolicySnapshot(ToolBarDateRunnerPolicyDecision decision, ToolBarDateRunnerFlowPolicy flowPolicy)
        {
            this.Decision = decision;
            this.FlowPolicy = flowPolicy;
        }

        public ToolBarDateRunnerPolicyDecision Decision { get; }
        public ToolBarDateRunnerFlowPolicy FlowPolicy { get; }
    }
}
