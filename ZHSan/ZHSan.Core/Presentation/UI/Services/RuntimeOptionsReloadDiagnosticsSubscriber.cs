using System;
using ZHSan.Core.Infrastructure.Configuration;
using ZHSan.Core.Infrastructure.Logging;

namespace ZHSan.Core.Presentation.UI.Services
{
    /// <summary>
    /// Secondary RuntimeOptions reload handler example.
    /// Demonstrates non-MainGameScreen listeners through RuntimeOptionsReloadCoordinator.
    /// </summary>
    public sealed class RuntimeOptionsReloadDiagnosticsSubscriber : IRuntimeOptionsReloadHandler, IDisposable
    {
        public void ApplyRuntimeOptions(RuntimeOptions options, string source)
        {
            if (options == null) return;
            RuntimeLog.Info(string.Format(
                "[RuntimeOptionsReloadSubscriber] Reload observed from {0}. DebugOverlay={1}.",
                string.IsNullOrWhiteSpace(source) ? "unknown" : source,
                options.Debug?.ShowDebugOverlay == true ? "ON" : "OFF"));
        }

        public void Dispose()
        {
        }
    }
}
