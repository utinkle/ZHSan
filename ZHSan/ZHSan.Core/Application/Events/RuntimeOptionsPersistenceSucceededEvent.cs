namespace ZHSan.Core.Application.Events
{
    public sealed class RuntimeOptionsPersistenceSucceededEvent
    {
        public RuntimeOptionsPersistenceSucceededEvent(string path, int attempt)
        {
            this.Path = path;
            this.Attempt = attempt;
        }

        public string Path { get; }

        public int Attempt { get; }
    }
}
