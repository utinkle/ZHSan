using System;
using System.Collections.Generic;

namespace ZHSan.Core.Infrastructure.Composition
{
    /// <summary>
    /// Lightweight service registry for phased refactor migration.
    /// </summary>
    public sealed class ServiceRegistry
    {
        private readonly Dictionary<Type, Func<object>> factories = new Dictionary<Type, Func<object>>();

        public void RegisterSingleton<TService>(TService instance) where TService : class
        {
            if (instance == null) throw new ArgumentNullException(nameof(instance));
            factories[typeof(TService)] = () => instance;
        }

        public void RegisterFactory<TService>(Func<TService> factory) where TService : class
        {
            if (factory == null) throw new ArgumentNullException(nameof(factory));
            factories[typeof(TService)] = () => factory();
        }

        public TService Resolve<TService>() where TService : class
        {
            if (!factories.TryGetValue(typeof(TService), out var factory))
            {
                return null;
            }

            return factory() as TService;
        }
    }
}
