using System;
using System.IO;
using System.Text.Json;
using ZHSan.Core.Application.Events;
using ZHSan.Core.Infrastructure.Logging;

namespace ZHSan.Core.Infrastructure.Configuration
{
    public sealed class RuntimeOptionsPersistenceService
    {
        private readonly IEventBus eventBus;

        public RuntimeOptionsPersistenceService(IEventBus eventBus)
        {
            this.eventBus = eventBus;
        }

        public bool TryPersist(RuntimeOptions options, string path)
        {
            if (options == null || string.IsNullOrWhiteSpace(path)) return false;

            var policy = options.Ui?.ToolBarDateRunnerPolicy;
            var retryEnabled = policy?.DiagnosticsOverlayPersistenceRetryEnabled ?? true;
            var retryCount = policy?.DiagnosticsOverlayPersistenceRetryCount ?? 1;
            if (!retryEnabled) retryCount = 0;

            var attempt = 0;
            while (true)
            {
                attempt++;
                try
                {
                    var json = JsonSerializer.Serialize(options, new JsonSerializerOptions { WriteIndented = true });
                    File.WriteAllText(path, json);
                    RuntimeLog.Info(string.Format("[ToolBarDateRunnerPolicy] RuntimeOptions persisted to {0} (attempt {1}).", path, attempt));
                    this.eventBus?.Publish(new RuntimeOptionsPersistenceSucceededEvent(path, attempt));
                    return true;
                }
                catch (Exception ex)
                {
                    var maxAttempts = retryCount + 1;
                    RuntimeLog.Info(string.Format("[ToolBarDateRunnerPolicy] RuntimeOptions persistence failed (attempt {0}/{1}). Path={2}, ErrorType={3}, Message={4}", attempt, maxAttempts, path, ex.GetType().Name, ex.Message));
                    this.eventBus?.Publish(new RuntimeOptionsPersistenceFailedEvent(path, ex.GetType().Name, ex.Message, attempt, maxAttempts));
                    if (attempt > retryCount)
                    {
                        RuntimeLog.Info("[ToolBarDateRunnerPolicy] RuntimeOptions persistence exhausted retries. Keep in-memory preset only.");
                        return false;
                    }
                }
            }
        }
    }
}
