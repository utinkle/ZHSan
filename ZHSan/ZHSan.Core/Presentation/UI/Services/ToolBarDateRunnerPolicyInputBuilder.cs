using WorldOfTheThreeKingdoms.GameScreens;

namespace ZHSan.Core.Presentation.UI.Services
{
    public sealed class ToolBarDateRunnerPolicyInputBuilder
    {
        public ToolBarDateRunnerPolicyInput Build(MainGameScreen screen)
        {
            if (screen == null || screen.Plugins == null) return default;

            var hasModalDialog = (screen.Plugins.ConfirmationDialogPlugin != null && screen.Plugins.ConfirmationDialogPlugin.IsShowing)
                || (screen.Plugins.HelpPlugin != null && screen.Plugins.HelpPlugin.IsShowing);
            var optionDialog = screen.Plugins.OptionDialogPlugin as OptionDialogPlugin.OptionDialogPlugin;
            var isOptionDialogShowing = optionDialog != null && optionDialog.IsShowing;
            var isContextMenuShowing = screen.Plugins.ContextMenuPlugin != null && screen.Plugins.ContextMenuPlugin.IsShowing;
            var hasFrameOverlay = screen.Plugins.GameFramePlugin != null && screen.Plugins.GameFramePlugin.IsShowing;
            var isMapSelectorShowing = screen.Plugins.MapViewSelectorPlugin != null && screen.Plugins.MapViewSelectorPlugin.IsShowing;

            return new ToolBarDateRunnerPolicyInput(hasModalDialog, isOptionDialogShowing, isContextMenuShowing, hasFrameOverlay, isMapSelectorShowing);
        }
    }
}
