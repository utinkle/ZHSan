using ZHSan.Core.Application.Events;
using ZHSan.Core.Infrastructure.Composition;
using ZHSan.Core.Infrastructure.FeatureFlags;

namespace ZHSan.Core.Infrastructure.Runtime
{
    public static class RuntimeBootstrap
    {
        public static ServiceRegistry Services { get; private set; }

        public static void Initialize()
        {
            if (Services != null) return;

            Services = new ServiceRegistry();
            Services.RegisterSingleton(FeatureFlags.Default());
            Services.RegisterSingleton<IEventBus>(new SimpleEventBus());
        }
    }
}
