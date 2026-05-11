using System.Collections.Generic;
using Microsoft.Xna.Framework;

namespace ZHSan.Core.Presentation.UI.ViewModels
{
    public sealed class ToolBarDateRunnerPolicyDebugOverlayViewModel
    {
        public List<string> Messages { get; set; } = new List<string>();

        public int FontSize { get; set; }

        public int LineSpacing { get; set; }

        public Color TextColor { get; set; }

        public float BackgroundOpacity { get; set; } = 0.65f;

        public int ScrollOffset { get; set; }

        public int MaxVisibleLines { get; set; } = 10;
    }
}
