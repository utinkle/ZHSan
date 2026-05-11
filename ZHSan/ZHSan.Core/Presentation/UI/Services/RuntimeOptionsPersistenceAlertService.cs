using System;
using ZHSan.Core.Application.Events;
using ZHSan.Core.Infrastructure.Configuration;
using ZHSan.Core.Presentation.UI.Events;

namespace ZHSan.Core.Presentation.UI.Services
{
    /// <summary>
    /// Classifies runtime-options persistence failures and emits overlay-friendly diagnostics alerts.
    /// </summary>
    public sealed class RuntimeOptionsPersistenceAlertService : IDisposable, IRuntimeOptionsReloadHandler
    {
        private readonly IEventBus eventBus;
        private RuntimeOptions runtimeOptions;
        private readonly IDisposable failureSubscription;
        private readonly IDisposable successSubscription;
        private int consecutiveFailureCount;
        private int thresholdHitCount;
        private int emittedAlertCount;
        private int quietWindowSuppressedCount;
        private DateTime lastAlertAtUtc = DateTime.MinValue;

        public RuntimeOptionsPersistenceAlertService(IEventBus eventBus, RuntimeOptions runtimeOptions)
        {
            this.eventBus = eventBus;
            this.runtimeOptions = runtimeOptions;
            if (eventBus != null)
            {
                this.failureSubscription = eventBus.Subscribe<RuntimeOptionsPersistenceFailedEvent>(this.OnPersistenceFailed);
                this.successSubscription = eventBus.Subscribe<RuntimeOptionsPersistenceSucceededEvent>(this.OnPersistenceSucceeded);
            }
        }

        public int ConsecutiveFailureCount => this.consecutiveFailureCount;

        public int ThresholdHitCount => this.thresholdHitCount;

        public int EmittedAlertCount => this.emittedAlertCount;

        public int QuietWindowSuppressedCount => this.quietWindowSuppressedCount;

        private void OnPersistenceFailed(RuntimeOptionsPersistenceFailedEvent evt)
        {
            if (evt == null) return;
            this.consecutiveFailureCount++;
            var policy = this.GetPolicy();
            if (!this.HasReachedThreshold(policy))
            {
                return;
            }

            this.thresholdHitCount++;
            this.PublishThresholdAlert(
                this.consecutiveFailureCount <= 1 ? "FirstFailure" : "ConsecutiveFailure",
                evt.Path,
                evt.ErrorType,
                evt.Message,
                evt.Attempt,
                evt.MaxAttempts,
                policy,
                "PersistenceFailed");
        }

        private void OnPersistenceSucceeded(RuntimeOptionsPersistenceSucceededEvent evt)
        {
            if (evt == null) return;
            if (this.consecutiveFailureCount <= 0) return;
            var previous = this.consecutiveFailureCount;
            this.consecutiveFailureCount = 0;
            this.lastAlertAtUtc = DateTime.MinValue;
            var policy = this.GetPolicy();
            var snapshot = this.CreateSnapshot(
                "Recovery",
                "PersistenceSucceeded",
                evt.Path,
                string.Empty,
                string.Format("recovered after {0} consecutive persistence failures", previous),
                evt.Attempt,
                0,
                policy);
            var message = string.Format(
                "[RuntimeOptionsPersistAlert] Level=Recovery, ResetFrom={0}, SuccessAttempt={1}, Path={2}, ThresholdHits={3}, EmittedAlerts={4}, QuietSuppressed={5}",
                previous,
                evt.Attempt,
                evt.Path,
                this.thresholdHitCount,
                this.emittedAlertCount,
                this.quietWindowSuppressedCount);
            this.eventBus?.Publish(new ToolBarDateRunnerPolicyDiagnosticsEvent(
                "PersistenceAlert",
                "Recovery",
                default,
                message,
                snapshot));
        }

        public void ApplyRuntimeOptions(RuntimeOptions options, string source)
        {
            if (options == null) return;
            this.runtimeOptions = options;
            var policy = this.GetPolicy();
            source = string.IsNullOrWhiteSpace(source) ? "unknown" : source;
            var appliedMessage = string.Format(
                "[RuntimeOptionsPersistAlert] Level=ConfigReload, Source={0}, Threshold={1}, QuietWindowMs={2}, Consecutive={3}, ThresholdHits={4}, EmittedAlerts={5}, QuietSuppressed={6}",
                source,
                this.GetThreshold(policy),
                this.GetQuietWindowMs(policy),
                this.consecutiveFailureCount,
                this.thresholdHitCount,
                this.emittedAlertCount,
                this.quietWindowSuppressedCount);
            var reloadSnapshot = this.CreateSnapshot(
                "ConfigReload",
                source,
                string.Empty,
                string.Empty,
                "runtime options reloaded",
                0,
                0,
                policy);
            this.eventBus?.Publish(new ToolBarDateRunnerPolicyDiagnosticsEvent(
                "PersistenceAlert",
                "ConfigReload",
                default,
                appliedMessage,
                reloadSnapshot));

            if (!this.HasReachedThreshold(policy))
            {
                return;
            }

            this.thresholdHitCount++;
            this.PublishThresholdAlert(
                "ConfigReloadThresholdMatch",
                string.Empty,
                string.Empty,
                string.Format("reloaded threshold is already satisfied by {0} consecutive persistence failures", this.consecutiveFailureCount),
                0,
                0,
                policy,
                source);
        }

        private ToolBarDateRunnerPolicyOptions GetPolicy()
        {
            return this.runtimeOptions?.Ui?.ToolBarDateRunnerPolicy;
        }

        private bool HasReachedThreshold(ToolBarDateRunnerPolicyOptions policy)
        {
            return this.consecutiveFailureCount >= this.GetThreshold(policy);
        }

        private int GetThreshold(ToolBarDateRunnerPolicyOptions policy)
        {
            var threshold = policy?.DiagnosticsOverlayPersistenceAlertThreshold ?? 1;
            return threshold <= 0 ? 1 : threshold;
        }

        private int GetQuietWindowMs(ToolBarDateRunnerPolicyOptions policy)
        {
            var quietWindowMs = policy?.DiagnosticsOverlayPersistenceAlertQuietWindowMs ?? 0;
            return quietWindowMs < 0 ? 0 : quietWindowMs;
        }

        private void PublishThresholdAlert(
            string level,
            string path,
            string errorType,
            string message,
            int attempt,
            int maxAttempts,
            ToolBarDateRunnerPolicyOptions policy,
            string trigger)
        {
            var quietWindowMs = this.GetQuietWindowMs(policy);
            if (quietWindowMs > 0)
            {
                var now = DateTime.UtcNow;
                var elapsed = (now - this.lastAlertAtUtc).TotalMilliseconds;
                if (elapsed < quietWindowMs)
                {
                    this.quietWindowSuppressedCount++;
                    return;
                }

                this.lastAlertAtUtc = now;
            }

            this.emittedAlertCount++;
            var alertMessage = string.Format(
                "[RuntimeOptionsPersistAlert] Level={0}, Trigger={1}, Consecutive={2}, Threshold={3}, ThresholdHits={4}, EmittedAlerts={5}, QuietSuppressed={6}, Attempt={7}/{8}, Path={9}, Error={10}: {11}",
                level,
                string.IsNullOrWhiteSpace(trigger) ? "unknown" : trigger,
                this.consecutiveFailureCount,
                this.GetThreshold(policy),
                this.thresholdHitCount,
                this.emittedAlertCount,
                this.quietWindowSuppressedCount,
                attempt,
                maxAttempts,
                string.IsNullOrWhiteSpace(path) ? "n/a" : path,
                string.IsNullOrWhiteSpace(errorType) ? "n/a" : errorType,
                message ?? string.Empty);
            var snapshot = this.CreateSnapshot(
                level,
                trigger,
                path,
                errorType,
                message,
                attempt,
                maxAttempts,
                policy);
            this.eventBus?.Publish(new ToolBarDateRunnerPolicyDiagnosticsEvent(
                "PersistenceAlert",
                level,
                default,
                alertMessage,
                snapshot));
        }

        private RuntimeOptionsPersistenceAlertSnapshot CreateSnapshot(
            string level,
            string trigger,
            string path,
            string errorType,
            string message,
            int attempt,
            int maxAttempts,
            ToolBarDateRunnerPolicyOptions policy)
        {
            return new RuntimeOptionsPersistenceAlertSnapshot(
                level,
                trigger,
                this.consecutiveFailureCount,
                this.GetThreshold(policy),
                this.GetQuietWindowMs(policy),
                this.thresholdHitCount,
                this.emittedAlertCount,
                this.quietWindowSuppressedCount,
                attempt,
                maxAttempts,
                path,
                errorType,
                message);
        }

        public void Dispose()
        {
            this.failureSubscription?.Dispose();
            this.successSubscription?.Dispose();
        }
    }
}
