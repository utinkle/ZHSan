namespace ZHSan.Core.Application.Events
{
    public sealed class RuntimeOptionsPersistenceFailedEvent
    {
        public RuntimeOptionsPersistenceFailedEvent(string path, string errorType, string message, int attempt, int maxAttempts)
        {
            this.Path = path;
            this.ErrorType = errorType;
            this.Message = message;
            this.Attempt = attempt;
            this.MaxAttempts = maxAttempts;
        }

        public string Path { get; }
        public string ErrorType { get; }
        public string Message { get; }
        public int Attempt { get; }
        public int MaxAttempts { get; }
    }
}
