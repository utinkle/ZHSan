using System;
using ZHSan.Core.Application.Events;
using ZHSan.Core.Infrastructure.Configuration;

namespace ZHSan.Core.Presentation.UI.Services
{
    /// <summary>
    /// Classifies runtime-options persistence failures and emits overlay-friendly diagnostics alerts.
    /// </summary>
    public sealed class RuntimeOptionsPersistenceAlertService : IDisposable
    {
        private readonly IEventBus eventBus;
        private readonly RuntimeOptions runtimeOptions;
        private readonly IDisposable subscription;
        private readonly IDisposable successSubscription;
        private int consecutiveFailureCount;
        private DateTime lastAlertAtUtc = DateTime.MinValue;

        public RuntimeOptionsPersistenceAlertService(IEventBus eventBus, RuntimeOptions runtimeOptions)
        {
            this.eventBus = eventBus;
            this.runtimeOptions = runtimeOptions;
            if (eventBus != null)
            {
                this.subscription = eventBus.Subscribe<RuntimeOptionsPersistenceFailedEvent>(this.OnPersistenceFailed);
                this.successSubscription = eventBus.Subscribe<RuntimeOptionsPersistenceSucceededEvent>(this.OnPersistenceSucceeded);
            }
        }

        private void OnPersistenceFailed(RuntimeOptionsPersistenceFailedEvent evt)
        {
            if (evt == null) return;
            this.consecutiveFailureCount++;
            var policy = this.runtimeOptions?.Ui?.ToolBarDateRunnerPolicy;
            var threshold = policy?.DiagnosticsOverlayPersistenceAlertThreshold ?? 1;
            var quietWindowMs = policy?.DiagnosticsOverlayPersistenceAlertQuietWindowMs ?? 0;
            if (this.consecutiveFailureCount < threshold)
            {
                return;
            }

            if (quietWindowMs > 0)
            {
                var now = DateTime.UtcNow;
                var elapsed = (now - this.lastAlertAtUtc).TotalMilliseconds;
                if (elapsed < quietWindowMs)
                {
                    return;
                }

                this.lastAlertAtUtc = now;
            }

            var level = this.consecutiveFailureCount <= 1 ? "FirstFailure" : "ConsecutiveFailure";
            var message = string.Format(
                "[RuntimeOptionsPersistAlert] Level={0}, Consecutive={1}, Attempt={2}/{3}, Path={4}, Error={5}: {6}",
                level,
                this.consecutiveFailureCount,
                evt.Attempt,
                evt.MaxAttempts,
                evt.Path,
                evt.ErrorType,
                evt.Message);
            this.eventBus?.Publish(new UI.Events.ToolBarDateRunnerPolicyDiagnosticsEvent(
                "PersistenceAlert",
                level,
                default,
                message));
        }

        private void OnPersistenceSucceeded(RuntimeOptionsPersistenceSucceededEvent evt)
        {
            if (evt == null) return;
            if (this.consecutiveFailureCount <= 0) return;
            var previous = this.consecutiveFailureCount;
            this.consecutiveFailureCount = 0;
            this.lastAlertAtUtc = DateTime.MinValue;
            var message = string.Format(
                "[RuntimeOptionsPersistAlert] Level=Recovery, ResetFrom={0}, SuccessAttempt={1}, Path={2}",
                previous,
                evt.Attempt,
                evt.Path);
            this.eventBus?.Publish(new UI.Events.ToolBarDateRunnerPolicyDiagnosticsEvent(
                "PersistenceAlert",
                "Recovery",
                default,
                message));
        }

        public void Dispose()
        {
            this.subscription?.Dispose();
            this.successSubscription?.Dispose();
        }
    }
}
