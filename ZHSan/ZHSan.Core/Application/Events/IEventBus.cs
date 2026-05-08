using System;

namespace ZHSan.Core.Application.Events
{
    public interface IEventBus
    {
        void Publish<TEvent>(TEvent evt) where TEvent : class;

        IDisposable Subscribe<TEvent>(Action<TEvent> handler) where TEvent : class;
    }
}
