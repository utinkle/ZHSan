using System;

namespace ZHSan.Core.Infrastructure.Logging
{
    public static class RuntimeLog
    {
        public static Action<string> Writer { get; set; } = message => Console.WriteLine(message);

        public static void Info(string message)
        {
            Writer?.Invoke(message);
        }
    }
}
