using System.Collections.Generic;

namespace ZHSan.Core.Presentation.UI
{
    public sealed class UiNavigationService
    {
        private readonly List<string> focusOrder = new List<string>();
        private int focusIndex = -1;

        public string CurrentFocusId => focusIndex >= 0 && focusIndex < focusOrder.Count ? focusOrder[focusIndex] : null;

        public void SetFocusOrder(IEnumerable<string> ids)
        {
            focusOrder.Clear();
            if (ids != null)
            {
                focusOrder.AddRange(ids);
            }

            focusIndex = focusOrder.Count > 0 ? 0 : -1;
        }

        public string FocusNext()
        {
            if (focusOrder.Count == 0) return null;
            focusIndex = (focusIndex + 1) % focusOrder.Count;
            return CurrentFocusId;
        }

        public string FocusPrevious()
        {
            if (focusOrder.Count == 0) return null;
            focusIndex = (focusIndex - 1 + focusOrder.Count) % focusOrder.Count;
            return CurrentFocusId;
        }
    }
}
