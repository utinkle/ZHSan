using GameGlobal;
using System.Collections.Generic;

namespace ZHSan.Core.Presentation.UI.TabList
{
    public sealed class TabListQueryProfileOverride
    {
        public List<TabListColumnDescriptor> Columns { get; set; }
        public TabListSortDescriptor Sort { get; set; }
        public int? PageSize { get; set; }
        public string StatusFilterMode { get; set; }
        public string NameFilterMode { get; set; }
    }

    public sealed class TabListQueryProfileProvider
    {
        private readonly Dictionary<string, TabListQueryProfileOverride> overrides = new Dictionary<string, TabListQueryProfileOverride>();

        public void RegisterOverride(FrameKind kind, TabListQueryProfileOverride profileOverride)
        {
            RegisterOverride(kind, null, profileOverride);
        }

        public void RegisterOverride(FrameKind kind, FrameFunction? function, TabListQueryProfileOverride profileOverride)
        {
            if (profileOverride == null) return;
            this.overrides[BuildOverrideKey(kind, function)] = profileOverride;
        }

        public void RegisterOverrides(IDictionary<FrameKind, TabListQueryProfileOverride> profileOverrides)
        {
            if (profileOverrides == null) return;
            foreach (var pair in profileOverrides)
            {
                RegisterOverride(pair.Key, pair.Value);
            }
        }

        public void RegisterFunctionOverrides(IDictionary<string, TabListQueryProfileOverride> profileOverrides)
        {
            if (profileOverrides == null) return;
            foreach (var pair in profileOverrides)
            {
                if (string.IsNullOrWhiteSpace(pair.Key) || pair.Value == null) continue;
                this.overrides[pair.Key] = pair.Value;
            }
        }

        public TabListQueryDescriptor CreateDefaultQuery(FrameKind kind, FrameFunction function, TabListDescriptorService descriptorService, string title, string tabName)
        {
            if (descriptorService == null) return null;
            var profileOverride = ResolveOverride(kind, function);
            var columns = profileOverride != null && profileOverride.Columns != null && profileOverride.Columns.Count > 0
                ? profileOverride.Columns
                : new List<TabListColumnDescriptor>
                {
                    new TabListColumnDescriptor { Id = "name", Title = "名称", Width = 220, Visible = true, Numeric = false, SmallToBig = true },
                    new TabListColumnDescriptor { Id = "status", Title = "状态", Width = 120, Visible = true, Numeric = false, SmallToBig = true }
                };

            var sort = profileOverride != null && profileOverride.Sort != null
                ? profileOverride.Sort
                : new TabListSortDescriptor { ColumnId = "name", Descending = false };

            var query = descriptorService.CreateQuery(columns, sort);
            ApplyFilters(kind, function, query, descriptorService, title, tabName);
            ApplyFieldMappings(kind, function, query, descriptorService);
            descriptorService.SetPage(query, 0, ResolvePageSize(kind, function));
            return query;
        }

        public void ApplyFilters(FrameKind kind, FrameFunction? function, TabListQueryDescriptor query, TabListDescriptorService descriptorService, string title, string tabName)
        {
            if (query == null || descriptorService == null) return;

            if (!string.IsNullOrWhiteSpace(title))
            {
                descriptorService.AddFilter(query, "status", title, ResolveStatusFilterMode(kind, function));
            }

            if (ShouldApplyTabNameAsFilter(function, tabName))
            {
                descriptorService.AddFilter(query, "name", tabName, ResolveNameFilterMode(kind, function));
            }
        }

        public void ApplyFieldMappings(FrameKind kind, FrameFunction? function, TabListQueryDescriptor query, TabListDescriptorService descriptorService)
        {
            if (query == null || descriptorService == null) return;
            var profileOverride = ResolveOverride(kind, function);
            if (profileOverride != null && profileOverride.Columns != null && profileOverride.Columns.Count > 0)
            {
                return;
            }

            descriptorService.AddFieldMapping(query, "name", "Name");
            descriptorService.AddFieldMapping(query, "status", null, useToString: true);

            switch (kind)
            {
                case FrameKind.Person:
                    descriptorService.AddFieldMapping(query, "faction", "BelongedFaction");
                    descriptorService.AddFieldMapping(query, "location", "LocationArchitecture");
                    break;
                case FrameKind.Troop:
                    descriptorService.AddFieldMapping(query, "faction", "BelongedFaction");
                    descriptorService.AddFieldMapping(query, "location", "StartingArchitecture");
                    break;
                case FrameKind.Architecture:
                    descriptorService.AddFieldMapping(query, "faction", "BelongedFaction");
                    break;
                case FrameKind.Treasure:
                    descriptorService.AddFieldMapping(query, "owner", "BelongedPerson");
                    break;
            }
        }



        private static bool ShouldApplyTabNameAsFilter(FrameFunction? function, string tabName)
        {
            if (string.IsNullOrWhiteSpace(tabName)) return false;

            // tabName 在大部分 FrameFunction 中表达“UI 页签语义”，而非“名称关键字过滤”。
            // 仅在 Browse 场景下按关键字过滤，以避免 Personal/运兵/Ability 等页签词误伤数据集。
            return function.HasValue && function.Value == FrameFunction.Browse;
        }
        private string ResolveStatusFilterMode(FrameKind kind, FrameFunction? function)
        {
            var profileOverride = ResolveOverride(kind, function);
            if (profileOverride != null && !string.IsNullOrWhiteSpace(profileOverride.StatusFilterMode))
            {
                return profileOverride.StatusFilterMode;
            }
            switch (kind)
            {
                case FrameKind.Person:
                case FrameKind.Troop:
                case FrameKind.Architecture:
                case FrameKind.Treasure:
                    return "exact";
                default:
                    return "contains";
            }
        }

        private string ResolveNameFilterMode(FrameKind kind, FrameFunction? function)
        {
            var profileOverride = ResolveOverride(kind, function);
            if (profileOverride != null && !string.IsNullOrWhiteSpace(profileOverride.NameFilterMode))
            {
                return profileOverride.NameFilterMode;
            }
            switch (kind)
            {
                case FrameKind.Person:
                case FrameKind.Troop:
                    return "prefix";
                default:
                    return "contains";
            }
        }

        private int ResolvePageSize(FrameKind kind, FrameFunction function)
        {
            var profileOverride = ResolveOverride(kind, function);
            if (profileOverride != null && profileOverride.PageSize.HasValue && profileOverride.PageSize.Value > 0)
            {
                return profileOverride.PageSize.Value;
            }

            if (function == FrameFunction.Browse && (kind == FrameKind.Person || kind == FrameKind.Troop || kind == FrameKind.Architecture || kind == FrameKind.Treasure))
            {
                return 30;
            }
            switch (kind)
            {
                case FrameKind.Person:
                case FrameKind.Troop:
                    return 25;
                default:
                    return 20;
            }
        }

        private TabListQueryProfileOverride ResolveOverride(FrameKind kind, FrameFunction? function)
        {
            TabListQueryProfileOverride result;
            if (function.HasValue && this.overrides.TryGetValue(BuildOverrideKey(kind, function), out result))
            {
                return result;
            }

            return this.overrides.TryGetValue(BuildOverrideKey(kind, null), out result) ? result : null;
        }

        public static string BuildOverrideKey(FrameKind kind, FrameFunction? function)
        {
            return function.HasValue ? kind + ":" + function.Value : kind.ToString();
        }
    }
}
