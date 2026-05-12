using ZHSan.Core.Infrastructure.FeatureFlags;

namespace ZHSan.Core.Presentation.UI
{
    /// <summary>
    /// Myra runtime scaffold. Full Myra Desktop initialization will be connected in Phase B implementation.
    /// </summary>
    public sealed class MyraUiRuntime
    {
        private readonly FeatureFlags featureFlags;

        public UiLayerManager Layers { get; } = new UiLayerManager();

        public bool IsEnabled => featureFlags != null && featureFlags.UseMyraUiRuntime;

        public MyraUiRuntime(FeatureFlags featureFlags)
        {
            this.featureFlags = featureFlags;
        }

        public void Initialize()
        {
            // Phase B scaffold only; Myra Desktop root binding to be added in next step.
        }

        public void Update()
        {
            // Reserved for input pumping / focus updates.
        }

        public void Draw()
        {
            // Reserved for Myra Desktop draw.
        }
    }
}
