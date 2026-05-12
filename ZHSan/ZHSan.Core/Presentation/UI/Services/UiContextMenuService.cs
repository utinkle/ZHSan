using Microsoft.Xna.Framework;
using PluginInterface;
using ZHSan.Core.Presentation.UI.Commands;

namespace ZHSan.Core.Presentation.UI.Services
{
    public sealed class UiContextMenuService
    {
        private readonly ContextMenuCommandBridge commandBridge;

        public UiContextMenuService(ContextMenuCommandBridge commandBridge)
        {
            this.commandBridge = commandBridge;
        }

        public void OpenArchitectureTroopLeftClick(IGameContextMenu contextMenu, object currentObject, Point anchor, Point viewport)
        {
            if (contextMenu == null) return;
            contextMenu.SetCurrentGameObject(currentObject);
            commandBridge.Execute(contextMenu,
                new UiCommandDescriptor { CommandId = "architecture_troop_left_click", MenuKindName = "ArchitectureTroopLeftClick" },
                anchor,
                viewport);
        }

        public void OpenTroopLeftClick(IGameContextMenu contextMenu, object currentObject, Point anchor, Point viewport)
        {
            if (contextMenu == null) return;
            contextMenu.SetCurrentGameObject(currentObject);
            commandBridge.Execute(contextMenu,
                new UiCommandDescriptor { CommandId = "troop_left_click", MenuKindName = "TroopLeftClick" },
                anchor,
                viewport);
        }

        public void OpenArchitectureLeftClick(IGameContextMenu contextMenu, object currentObject, Point anchor, Point viewport)
        {
            if (contextMenu == null) return;
            contextMenu.SetCurrentGameObject(currentObject);
            commandBridge.Execute(contextMenu,
                new UiCommandDescriptor { CommandId = "architecture_left_click", MenuKindName = "ArchitectureLeftClick" },
                anchor,
                viewport);
        }
    }
}
