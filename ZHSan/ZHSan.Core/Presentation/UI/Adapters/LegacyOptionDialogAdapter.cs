using PluginInterface;
using ZHSan.Core.Presentation.UI.ViewModels;

namespace ZHSan.Core.Presentation.UI.Adapters
{
    public sealed class LegacyOptionDialogAdapter : IMyraViewAdapter
    {
        private readonly IOptionDialog legacyDialog;
        private OptionDialogViewModel viewModel;

        public string ViewId => "option_dialog";

        public LegacyOptionDialogAdapter(IOptionDialog legacyDialog)
        {
            this.legacyDialog = legacyDialog;
        }

        public void Show()
        {
            if (legacyDialog == null) return;
            var position = viewModel?.Position ?? GameGlobal.ShowPosition.Center;
            legacyDialog.ShowOptionDialog(position);
        }

        public void Hide()
        {
            legacyDialog?.HideOptionDialog();
        }

        public void Bind(object viewModel)
        {
            this.viewModel = viewModel as OptionDialogViewModel;
            Refresh();
        }

        public void Refresh()
        {
            if (legacyDialog == null || viewModel == null) return;
            if (!string.IsNullOrWhiteSpace(viewModel.Style))
            {
                legacyDialog.SetStyle(viewModel.Style);
            }

            if (!string.IsNullOrWhiteSpace(viewModel.Title))
            {
                legacyDialog.SetTitle(viewModel.Title);
            }
        }

        public void Dispose()
        {
            Hide();
        }
    }
}
