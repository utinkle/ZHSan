using System;
using ZHSan.Core.Application.Events;
using ZHSan.Core.Infrastructure.Configuration;
using ZHSan.Core.Presentation.UI.Events;
using ZHSan.Core.Infrastructure.Logging;

namespace ZHSan.Core.Presentation.UI.Services
{
    /// <summary>
    /// Secondary RuntimeOptions reload handler example.
    /// Demonstrates non-MainGameScreen listeners through RuntimeOptionsReloadCoordinator.
    /// </summary>
    public sealed class RuntimeOptionsReloadDiagnosticsSubscriber : IRuntimeOptionsReloadHandler, IDisposable
    {
        private readonly IEventBus eventBus;

        public RuntimeOptionsReloadDiagnosticsSubscriber(IEventBus eventBus)
        {
            this.eventBus = eventBus;
        }

        public void ApplyRuntimeOptions(RuntimeOptions options, string source)
        {
            if (options == null) return;
            var reloadSnapshot = new ToolBarDateRunnerPolicyReloadDiagnosticsSnapshot(
                source,
                options.Debug?.ShowDebugOverlay == true);
            var message = string.Format(
                "[RuntimeOptionsReloadSubscriber] Reload observed from {0}. DebugOverlay={1}.",
                reloadSnapshot.Source,
                reloadSnapshot.DebugOverlayEnabled ? "ON" : "OFF");
            RuntimeLog.Info(message);
            this.eventBus?.Publish(new ToolBarDateRunnerPolicyDiagnosticsEvent(
                "Reload",
                reloadSnapshot.Source,
                default,
                message,
                null,
                null,
                null,
                reloadSnapshot));
        }

        public void Dispose()
        {
        }
    }
}
