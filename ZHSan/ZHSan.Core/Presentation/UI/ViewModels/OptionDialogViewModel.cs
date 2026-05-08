using GameGlobal;

namespace ZHSan.Core.Presentation.UI.ViewModels
{
    public sealed class OptionDialogViewModel
    {
        public ShowPosition Position { get; set; } = ShowPosition.Center;

        public string Style { get; set; }

        public string Title { get; set; }
    }
}
