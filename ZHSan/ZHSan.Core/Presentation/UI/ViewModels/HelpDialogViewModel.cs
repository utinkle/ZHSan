using GameGlobal;

namespace ZHSan.Core.Presentation.UI.ViewModels
{
    public sealed class HelpDialogViewModel
    {
        public ShowPosition Position { get; set; } = ShowPosition.Center;

        public string HelpKey { get; set; }
    }
}
