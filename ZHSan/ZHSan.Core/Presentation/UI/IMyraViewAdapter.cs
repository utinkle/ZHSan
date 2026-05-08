namespace ZHSan.Core.Presentation.UI
{
    public interface IMyraViewAdapter
    {
        string ViewId { get; }

        void Show();

        void Hide();

        void Bind(object viewModel);

        void Refresh();

        void Dispose();
    }
}
