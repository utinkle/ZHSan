namespace ZHSan.Core.Presentation.UI
{
    public sealed class UiThemeService
    {
        public UiStyleTokens CurrentTokens { get; private set; }

        public UiThemeService(UiStyleTokens tokens)
        {
            CurrentTokens = tokens ?? UiStyleTokens.Default();
        }

        public void Apply(UiStyleTokens tokens)
        {
            CurrentTokens = tokens ?? UiStyleTokens.Default();
        }
    }
}
