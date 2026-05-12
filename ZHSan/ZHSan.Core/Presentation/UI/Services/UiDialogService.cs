using PluginInterface;
using ZHSan.Core.Infrastructure.FeatureFlags;
using ZHSan.Core.Presentation.UI.Adapters;
using ZHSan.Core.Presentation.UI.ViewModels;

namespace ZHSan.Core.Presentation.UI.Services
{
    public sealed class UiDialogService
    {
        private readonly FeatureFlags featureFlags;
        private IMyraViewAdapter confirmationAdapter;
        private IMyraViewAdapter optionAdapter;
        private IMyraViewAdapter helpAdapter;

        public UiDialogService(FeatureFlags featureFlags)
        {
            this.featureFlags = featureFlags;
        }

        public void ShowConfirmation(IConfirmationDialog dialog, ConfirmationDialogViewModel viewModel)
        {
            if (dialog == null) return;
            if (featureFlags != null && featureFlags.UseMyraUiRuntime)
            {
                if (confirmationAdapter == null)
                {
                    confirmationAdapter = new LegacyConfirmationDialogAdapter(dialog);
                }

                confirmationAdapter.Bind(viewModel);
                confirmationAdapter.Show();
                return;
            }

            if (viewModel != null)
            {
                dialog.SetPosition(viewModel.Position);
                if (!string.IsNullOrWhiteSpace(viewModel.DescriptionText))
                {
                    dialog.SetDescriptionText(viewModel.DescriptionText);
                }
            }

            dialog.IsShowing = true;
        }

        public void ShowOption(IOptionDialog dialog, OptionDialogViewModel viewModel)
        {
            if (dialog == null) return;
            if (featureFlags != null && featureFlags.UseMyraUiRuntime)
            {
                if (optionAdapter == null)
                {
                    optionAdapter = new LegacyOptionDialogAdapter(dialog);
                }

                optionAdapter.Bind(viewModel);
                optionAdapter.Show();
                return;
            }

            if (viewModel != null)
            {
                dialog.SetStyle(viewModel.Style);
                dialog.SetTitle(viewModel.Title);
            }

            dialog.ShowOptionDialog(viewModel?.Position ?? GameGlobal.ShowPosition.Center);
        }

        public void ShowHelp(IHelp help, HelpDialogViewModel viewModel, UiThemeService themeService, UiNavigationService navigationService)
        {
            if (help == null) return;
            if (featureFlags != null && featureFlags.UseMyraUiRuntime)
            {
                if (helpAdapter == null)
                {
                    helpAdapter = new LegacyHelpDialogAdapter(help, themeService, navigationService);
                }

                helpAdapter.Bind(viewModel);
                helpAdapter.Show();
                return;
            }

            if (viewModel != null)
            {
                help.SetMapPosition(viewModel.Position);
                if (!string.IsNullOrWhiteSpace(viewModel.HelpKey))
                {
                    help.SetCurrentKey(viewModel.HelpKey);
                }
            }

            help.IsShowing = true;
        }
    }
}
