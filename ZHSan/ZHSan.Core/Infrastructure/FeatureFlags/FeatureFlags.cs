namespace ZHSan.Core.Infrastructure.FeatureFlags
{
    public sealed class FeatureFlags
    {
        // UI migration
        public bool UseMyraUiRuntime { get; set; }

        // Animation migration
        public bool UseNewAnimationSystem { get; set; }

        // Camera/effects migration
        public bool UseNewCameraSystem { get; set; }

        public static FeatureFlags Default()
        {
            return new FeatureFlags
            {
                UseMyraUiRuntime = false,
                UseNewAnimationSystem = false,
                UseNewCameraSystem = false
            };
        }
    }
}
