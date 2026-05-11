using ZHSan.Core.Infrastructure.Configuration;
using ZHSan.Core.Application.Events;
using ZHSan.Core.Infrastructure.Composition;
using ZHSan.Core.Infrastructure.FeatureFlags;
using ZHSan.Core.Presentation.UI.Commands;
using FeatureFlagsConfig = ZHSan.Core.Infrastructure.FeatureFlags.FeatureFlags;
using ZHSan.Core.Presentation.UI;
using ZHSan.Core.Presentation.UI.Services;
using ZHSan.Core.Presentation.UI.TabList;
using ZHSan.Core.Infrastructure.Logging;
using GameGlobal;
using System;
using System.Collections.Generic;

namespace ZHSan.Core.Infrastructure.Runtime
{
    public static class RuntimeBootstrap
    {
        public static ServiceRegistry Services { get; private set; }
        private const string RuntimeOptionsFilePath = @"Content\Data\RuntimeOptions.json";

        public static void Initialize()
        {
            if (Services != null) return;

            Services = new ServiceRegistry();
            Services.RegisterSingleton(FeatureFlagsConfig.Default());
            Services.RegisterSingleton<IEventBus>(new SimpleEventBus());
            Services.RegisterSingleton(RuntimeOptionsLoader.LoadOrDefault(RuntimeOptionsFilePath, RuntimeLog.Info));
            Services.RegisterSingleton(new RuntimeOptionsPersistenceService(Services.Resolve<IEventBus>()));

            var featureFlags = Services.Resolve<FeatureFlagsConfig>();
            Services.RegisterSingleton(new MyraUiRuntime(featureFlags));
            Services.RegisterSingleton(UiStyleTokens.Default());
            Services.RegisterSingleton(new UiThemeService(UiStyleTokens.Default()));
            Services.RegisterSingleton(new UiNavigationService());
            Services.RegisterSingleton(new UiDialogService(featureFlags));
            Services.RegisterSingleton(new UiContextMenuService(new ContextMenuCommandBridge()));
            Services.RegisterSingleton(new ToolBarDateRunnerInteractionService(Services.Resolve<RuntimeOptions>()?.Ui?.ToolBarDateRunnerPolicy?.SuspendOnOptionDialog ?? true));
            Services.RegisterSingleton(new ToolBarDateRunnerPolicyInputBuilder());
            Services.RegisterSingleton(new ToolBarDateRunnerFlowPolicyBuilder());
            Services.RegisterSingleton(new ToolBarDateRunnerPolicyCoordinator(
                Services.Resolve<ToolBarDateRunnerInteractionService>(),
                Services.Resolve<ToolBarDateRunnerPolicyInputBuilder>(),
                Services.Resolve<ToolBarDateRunnerFlowPolicyBuilder>()));
            Services.RegisterSingleton(new ToolBarDateRunnerPolicyDebugOverlay(
                Services.Resolve<IEventBus>(),
                Services.Resolve<RuntimeOptions>()));
            Services.RegisterSingleton(new RuntimeOptionsReloadDiagnosticsSubscriber(Services.Resolve<IEventBus>()));
            Services.RegisterSingleton(new RuntimeOptionsPersistenceAlertService(
                Services.Resolve<IEventBus>(),
                Services.Resolve<RuntimeOptions>()));
            Services.RegisterSingleton(new TabListDescriptorService());
            var tabListProfileProvider = new TabListQueryProfileProvider();
            ApplyTabListProfileOverrides(tabListProfileProvider, Services.Resolve<RuntimeOptions>());
            Services.RegisterSingleton(tabListProfileProvider);
        }

        private static void ApplyTabListProfileOverrides(TabListQueryProfileProvider provider, RuntimeOptions options)
        {
            if (provider == null || options == null || options.Ui == null || options.Ui.TabListProfiles == null || options.Ui.TabListProfiles.Overrides == null)
            {
                return;
            }

            foreach (var item in options.Ui.TabListProfiles.Overrides)
            {
                if (item == null || string.IsNullOrWhiteSpace(item.FrameKind)) continue;
                FrameKind frameKind;
                if (!Enum.TryParse(item.FrameKind, true, out frameKind)) continue;
                FrameFunction parsedFunction;
                FrameFunction? function = null;
                if (!string.IsNullOrWhiteSpace(item.FrameFunction) && Enum.TryParse(item.FrameFunction, true, out parsedFunction))
                {
                    function = parsedFunction;
                }

                provider.RegisterOverride(frameKind, function, new TabListQueryProfileOverride
                {
                    PageSize = item.PageSize,
                    NameFilterMode = item.NameFilterMode,
                    StatusFilterMode = item.StatusFilterMode,
                    Sort = item.Sort == null ? null : new TabListSortDescriptor
                    {
                        ColumnId = item.Sort.ColumnId,
                        Descending = item.Sort.Descending
                    },
                    Columns = item.Columns == null ? null : item.Columns.ConvertAll(c => new TabListColumnDescriptor
                    {
                        Id = c.Id,
                        Title = c.Title,
                        Width = c.Width,
                        Visible = c.Visible,
                        Numeric = c.Numeric,
                        SmallToBig = c.SmallToBig
                    })
                });
            }
        }
    }
}
