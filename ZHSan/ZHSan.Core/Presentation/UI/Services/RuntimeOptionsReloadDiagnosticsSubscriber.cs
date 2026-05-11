using System;
using ZHSan.Core.Application.Events;
using ZHSan.Core.Infrastructure.Logging;

namespace ZHSan.Core.Presentation.UI.Services
{
    /// <summary>
    /// Secondary subscriber example for RuntimeOptionsReloadedEvent.
    /// Demonstrates non-MainGameScreen listeners.
    /// </summary>
    public sealed class RuntimeOptionsReloadDiagnosticsSubscriber : IDisposable
    {
        private IDisposable subscription;
        private bool subscribed;

        public RuntimeOptionsReloadDiagnosticsSubscriber(IEventBus eventBus)
        {
            this.EnsureSubscribed(eventBus);
        }

        private void OnRuntimeOptionsReloaded(RuntimeOptionsReloadedEvent evt)
        {
            if (evt?.Options == null) return;
            RuntimeLog.Info(string.Format(
                "[RuntimeOptionsReloadSubscriber] Reload observed from {0}. DebugOverlay={1}.",
                string.IsNullOrWhiteSpace(evt.Source) ? "unknown" : evt.Source,
                evt.Options.Debug?.ShowDebugOverlay == true ? "ON" : "OFF"));
        }

        public void Dispose()
        {
            this.subscription?.Dispose();
            this.subscription = null;
            this.subscribed = false;
        }

        private void EnsureSubscribed(IEventBus eventBus)
        {
            if (this.subscribed)
            {
                RuntimeLog.Info("[RuntimeOptionsReloadSubscriber] Subscription already attached. Skip duplicate subscription.");
                return;
            }

            if (eventBus == null) return;
            this.subscription = eventBus.Subscribe<RuntimeOptionsReloadedEvent>(this.OnRuntimeOptionsReloaded);
            this.subscribed = this.subscription != null;
        }
    }
}
