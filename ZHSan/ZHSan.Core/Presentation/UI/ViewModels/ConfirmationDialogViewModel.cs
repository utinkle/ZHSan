using GameGlobal;

namespace ZHSan.Core.Presentation.UI.ViewModels
{
    public sealed class ConfirmationDialogViewModel
    {
        public ShowPosition Position { get; set; } = ShowPosition.Center;

        public string DescriptionText { get; set; }
    }
}
