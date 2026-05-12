using PluginInterface;
using ZHSan.Core.Presentation.UI.ViewModels;

namespace ZHSan.Core.Presentation.UI.Adapters
{
    /// <summary>
    /// Transitional adapter: wraps legacy confirmation dialog plugin behind IMyraViewAdapter contract.
    /// In Phase C this can be replaced by a real Myra implementation without changing callers.
    /// </summary>
    public sealed class LegacyConfirmationDialogAdapter : IMyraViewAdapter
    {
        private readonly IConfirmationDialog legacyDialog;
        private ConfirmationDialogViewModel viewModel;

        public string ViewId => "confirmation_dialog";

        public LegacyConfirmationDialogAdapter(IConfirmationDialog legacyDialog)
        {
            this.legacyDialog = legacyDialog;
        }

        public void Show()
        {
            if (legacyDialog == null) return;
            legacyDialog.IsShowing = true;
        }

        public void Hide()
        {
            if (legacyDialog == null) return;
            legacyDialog.IsShowing = false;
        }

        public void Bind(object viewModel)
        {
            this.viewModel = viewModel as ConfirmationDialogViewModel;
            Refresh();
        }

        public void Refresh()
        {
            if (legacyDialog == null || viewModel == null) return;
            legacyDialog.SetPosition(viewModel.Position);
            if (!string.IsNullOrWhiteSpace(viewModel.DescriptionText))
            {
                legacyDialog.SetDescriptionText(viewModel.DescriptionText);
            }
        }

        public void Dispose()
        {
            Hide();
        }
    }
}
