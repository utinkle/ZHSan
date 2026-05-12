using Microsoft.Xna.Framework;
using PluginInterface;

namespace ZHSan.Core.Presentation.UI.Commands
{
    public sealed class ContextMenuCommandBridge
    {
        public void Execute(IGameContextMenu contextMenu, UiCommandDescriptor command, Point anchor, Point viewport)
        {
            if (contextMenu == null || command == null) return;

            if (!string.IsNullOrWhiteSpace(command.MenuKindName))
            {
                contextMenu.SetMenuKindByName(command.MenuKindName);
            }
            else
            {
                contextMenu.SetMenuKindByID(command.ParamId);
            }

            contextMenu.Prepare(anchor.X, anchor.Y, viewport);
            contextMenu.IsShowing = true;
        }
    }
}
