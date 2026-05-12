using System.Collections.Generic;
using System.Linq;

namespace ZHSan.Core.Presentation.UI.TabList
{
    /// <summary>
    /// Pre-migration descriptor service for TabList modernization.
    /// Converts legacy definitions into neutral descriptors for future Myra grid/list rendering.
    /// </summary>
    public sealed class TabListDescriptorService
    {
        public TabListQueryDescriptor CreateQuery(IEnumerable<TabListColumnDescriptor> columns, TabListSortDescriptor sort = null)
        {
            var query = new TabListQueryDescriptor();
            if (columns != null)
            {
                query.Columns.AddRange(columns.Select(c => new TabListColumnDescriptor
                {
                    Id = c.Id,
                    Title = c.Title,
                    Width = c.Width,
                    Visible = c.Visible,
                    Numeric = c.Numeric,
                    SmallToBig = c.SmallToBig
                }));
            }

            query.Sort = sort;
            return query;
        }

        public void SetPage(TabListQueryDescriptor query, int pageIndex, int pageSize)
        {
            if (query == null || pageSize <= 0) return;
            query.Page = new TabListPageDescriptor
            {
                PageIndex = pageIndex < 0 ? 0 : pageIndex,
                PageSize = pageSize
            };
        }

        public void AddFilter(TabListQueryDescriptor query, string columnId, string keyword, string matchMode = "contains")
        {
            if (query == null || string.IsNullOrWhiteSpace(columnId)) return;
            query.Filters.Add(new TabListFilterDescriptor
            {
                ColumnId = columnId,
                Keyword = keyword,
                MatchMode = matchMode
            });
        }

        public void AddFieldMapping(TabListQueryDescriptor query, string columnId, string propertyName, bool useToString = false)
        {
            if (query == null || string.IsNullOrWhiteSpace(columnId)) return;
            query.FieldMappings.Add(new TabListFieldMappingDescriptor
            {
                ColumnId = columnId,
                PropertyName = propertyName,
                UseToString = useToString
            });
        }
    }
}
