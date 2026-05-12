using Microsoft.Xna.Framework;

namespace ZHSan.Core.Presentation.UI
{
    public sealed class UiStyleTokens
    {
        public Color PrimaryTextColor { get; set; } = Color.White;

        public Color AccentColor { get; set; } = new Color(0x3A, 0x86, 0xFF);

        public int BaseFontSize { get; set; } = 18;

        public int Spacing { get; set; } = 8;

        public int BorderThickness { get; set; } = 1;

        public static UiStyleTokens Default() => new UiStyleTokens();
    }
}
