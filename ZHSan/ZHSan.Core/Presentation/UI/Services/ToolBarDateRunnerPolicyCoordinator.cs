using ZHSan.Core.Infrastructure.Configuration;
using ZHSan.Core.Infrastructure.FeatureFlags;
using WorldOfTheThreeKingdoms.GameScreens;
using System;

namespace ZHSan.Core.Presentation.UI.Services
{
    public sealed class ToolBarDateRunnerPolicyCoordinator
    {
        private readonly ToolBarDateRunnerInteractionService interactionService;
        private readonly ToolBarDateRunnerPolicyInputBuilder inputBuilder;
        private readonly ToolBarDateRunnerFlowPolicyBuilder flowPolicyBuilder;
        private bool hasCache;
        private ToolBarDateRunnerPolicyInput lastInput;
        private ToolBarDateRunnerFlowPolicy lastFlowPolicy;
        private ToolBarDateRunnerPolicySnapshot lastSnapshot;
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

        public void InvalidateCache()
        {
            this.hasCache = false;
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
                "[ToolBarDateRunnerPolicy] Cache invalidated ({0}). Hits={1}, Misses={2}, HitRate={3:F2}%, MissRate={4:F2}%.",
                reason ?? "unspecified",
                this.CacheHitCount,
                this.CacheMissCount,
                hitRate,
                missRate);
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
