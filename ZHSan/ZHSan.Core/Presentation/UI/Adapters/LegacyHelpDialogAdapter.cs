using PluginInterface;
using ZHSan.Core.Presentation.UI;
using ZHSan.Core.Presentation.UI.ViewModels;

namespace ZHSan.Core.Presentation.UI.Adapters
{
    public sealed class LegacyHelpDialogAdapter : IMyraViewAdapter
    {
        private readonly IHelp legacyHelp;
        private HelpDialogViewModel viewModel;
        private readonly UiThemeService themeService;
        private readonly UiNavigationService navigationService;

        public string ViewId => "help_dialog";

        public LegacyHelpDialogAdapter(IHelp legacyHelp, UiThemeService themeService, UiNavigationService navigationService)
        {
            this.legacyHelp = legacyHelp;
            this.themeService = themeService;
            this.navigationService = navigationService;
        }

        public void Show()
        {
            if (legacyHelp == null) return;
            navigationService?.SetFocusOrder(new[] { ViewId + ".ok", ViewId + ".close" });
            legacyHelp.IsShowing = true;
        }

        public void Hide()
        {
            if (legacyHelp == null) return;
            legacyHelp.IsShowing = false;
        }

        public void Bind(object viewModel)
        {
            this.viewModel = viewModel as HelpDialogViewModel;
            Refresh();
        }

        public void Refresh()
        {
            if (legacyHelp == null || viewModel == null) return;
            var tokens = themeService?.CurrentTokens;
            legacyHelp.SetMapPosition(viewModel.Position);
            if (tokens != null)
            {
                legacyHelp.IsButtonShowing = tokens.BorderThickness > 0;
            }
            if (!string.IsNullOrWhiteSpace(viewModel.HelpKey))
            {
                legacyHelp.SetCurrentKey(viewModel.HelpKey);
            }
        }

        public void Dispose()
        {
            Hide();
        }
    }
}
