using System.Collections.Generic;

namespace ZHSan.Core.Presentation.UI.TabList
{
    public sealed class TabListColumnDescriptor
    {
        public string Id { get; set; }
        public string Title { get; set; }
        public int Width { get; set; }
        public bool? Visible { get; set; }
        public bool? Numeric { get; set; }
        public bool? SmallToBig { get; set; }
    }

    public sealed class TabListSortDescriptor
    {
        public string ColumnId { get; set; }
        public bool Descending { get; set; }
    }

    public sealed class TabListFilterDescriptor
    {
        public string ColumnId { get; set; }
        public string Keyword { get; set; }
        public string MatchMode { get; set; }
    }

    public sealed class TabListFieldMappingDescriptor
    {
        public string ColumnId { get; set; }
        public string PropertyName { get; set; }
        public bool UseToString { get; set; }
    }

    public sealed class TabListPageDescriptor
    {
        public int PageIndex { get; set; }
        public int PageSize { get; set; }
    }

    public sealed class TabListQueryDescriptor
    {
        public List<TabListColumnDescriptor> Columns { get; } = new List<TabListColumnDescriptor>();
        public TabListSortDescriptor Sort { get; set; }
        public List<TabListFilterDescriptor> Filters { get; } = new List<TabListFilterDescriptor>();
        public List<TabListFieldMappingDescriptor> FieldMappings { get; } = new List<TabListFieldMappingDescriptor>();
        public TabListPageDescriptor Page { get; set; }
    }
}
