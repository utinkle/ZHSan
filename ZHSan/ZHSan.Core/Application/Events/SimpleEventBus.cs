using System;
using System.Collections.Generic;

namespace ZHSan.Core.Application.Events
{
    public sealed class SimpleEventBus : IEventBus
    {
        private readonly Dictionary<Type, List<Delegate>> handlers = new Dictionary<Type, List<Delegate>>();

        public void Publish<TEvent>(TEvent evt) where TEvent : class
        {
            if (evt == null) return;

            if (!handlers.TryGetValue(typeof(TEvent), out var handlerList))
            {
                return;
            }

            foreach (var handler in handlerList.ToArray())
            {
                (handler as Action<TEvent>)?.Invoke(evt);
            }
        }

        public IDisposable Subscribe<TEvent>(Action<TEvent> handler) where TEvent : class
        {
            if (handler == null) throw new ArgumentNullException(nameof(handler));

            var eventType = typeof(TEvent);
            if (!handlers.TryGetValue(eventType, out var handlerList))
            {
                handlerList = new List<Delegate>();
                handlers.Add(eventType, handlerList);
            }

            handlerList.Add(handler);
            return new Subscription(() => handlerList.Remove(handler));
        }

        private sealed class Subscription : IDisposable
        {
            private readonly Action onDispose;
            private bool disposed;

            public Subscription(Action onDispose)
            {
                this.onDispose = onDispose;
            }

            public void Dispose()
            {
                if (disposed) return;
                disposed = true;
                onDispose?.Invoke();
            }
        }
    }
}
