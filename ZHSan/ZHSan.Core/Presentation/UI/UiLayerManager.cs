using System.Collections.Generic;
using System.Linq;

namespace ZHSan.Core.Presentation.UI
{
    public sealed class UiLayerManager
    {
        private readonly Dictionary<UiLayer, List<IMyraViewAdapter>> layers = new Dictionary<UiLayer, List<IMyraViewAdapter>>();

        public void Register(UiLayer layer, IMyraViewAdapter adapter)
        {
            if (adapter == null) return;
            if (!layers.TryGetValue(layer, out var entries))
            {
                entries = new List<IMyraViewAdapter>();
                layers[layer] = entries;
            }

            if (!entries.Contains(adapter))
            {
                entries.Add(adapter);
            }
        }

        public IReadOnlyList<IMyraViewAdapter> GetLayerViews(UiLayer layer)
        {
            if (layers.TryGetValue(layer, out var entries))
            {
                return entries;
            }

            return Enumerable.Empty<IMyraViewAdapter>().ToList();
        }
    }
}
