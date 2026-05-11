using ZHSan.Core.Infrastructure.Configuration;

namespace ZHSan.Core.Application.Events
{
    public sealed class RuntimeOptionsReloadedEvent
    {
        public RuntimeOptionsReloadedEvent(RuntimeOptions options, string source)
        {
            this.Options = options;
            this.Source = source;
        }

        public RuntimeOptions Options { get; }

        public string Source { get; }
    }
}
