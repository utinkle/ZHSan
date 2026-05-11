using System;
using System.Collections.Generic;
using ZHSan.Core.Application.Events;

namespace ZHSan.Core.Infrastructure.Configuration
{
    public interface IRuntimeOptionsReloadHandler
    {
        void ApplyRuntimeOptions(RuntimeOptions options, string source);
    }

    /// <summary>
    /// Centralizes RuntimeOptionsReloadedEvent subscriptions and fans reloaded options out to registered handlers.
    /// </summary>
    public sealed class RuntimeOptionsReloadCoordinator : IDisposable
    {
        private readonly List<IRuntimeOptionsReloadHandler> handlers = new List<IRuntimeOptionsReloadHandler>();
        private readonly IDisposable subscription;

        public RuntimeOptionsReloadCoordinator(IEventBus eventBus)
        {
            if (eventBus != null)
            {
                this.subscription = eventBus.Subscribe<RuntimeOptionsReloadedEvent>(this.OnRuntimeOptionsReloaded);
            }
        }

        public IDisposable RegisterHandler(IRuntimeOptionsReloadHandler handler)
        {
            if (handler == null) throw new ArgumentNullException(nameof(handler));
            if (!this.handlers.Contains(handler))
            {
                this.handlers.Add(handler);
            }

            return new HandlerRegistration(this.handlers, handler);
        }

        private void OnRuntimeOptionsReloaded(RuntimeOptionsReloadedEvent evt)
        {
            if (evt == null || evt.Options == null) return;
            var source = string.IsNullOrWhiteSpace(evt.Source) ? "unknown" : evt.Source;
            foreach (var handler in this.handlers.ToArray())
            {
                handler?.ApplyRuntimeOptions(evt.Options, source);
            }
        }

        public void Dispose()
        {
            this.subscription?.Dispose();
            this.handlers.Clear();
        }

        private sealed class HandlerRegistration : IDisposable
        {
            private readonly List<IRuntimeOptionsReloadHandler> handlers;
            private readonly IRuntimeOptionsReloadHandler handler;
            private bool disposed;

            public HandlerRegistration(List<IRuntimeOptionsReloadHandler> handlers, IRuntimeOptionsReloadHandler handler)
            {
                this.handlers = handlers;
                this.handler = handler;
            }

            public void Dispose()
            {
                if (this.disposed) return;
                this.disposed = true;
                this.handlers.Remove(this.handler);
            }
        }
    }
}
