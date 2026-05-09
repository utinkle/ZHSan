namespace ZHSan.Core.Presentation.UI.Services
{
    /// <summary>
    /// C-3 高风险迁移前置：集中定义 ToolBar 与 DateRunner 的交互策略，
    /// 先以只读策略服务落地，后续替换旧插件联动逻辑时可统一复用。
    /// </summary>
    public sealed class ToolBarDateRunnerInteractionService
    {
        private readonly bool suspendOnOptionDialog;

        public ToolBarDateRunnerInteractionService(bool suspendOnOptionDialog = true)
        {
            this.suspendOnOptionDialog = suspendOnOptionDialog;
        }

        public ToolBarDateRunnerPolicyDecision Evaluate(ToolBarDateRunnerPolicyInput input)
        {
            return new ToolBarDateRunnerPolicyDecision(
                suspendDateRunner: input.HasModalDialog || (this.suspendOnOptionDialog && input.IsOptionDialogShowing) || input.IsContextMenuShowing,
                lockToolBarInput: input.HasFrameOverlay || input.IsMapSelectorShowing);
        }
    }

    public readonly struct ToolBarDateRunnerPolicyInput
    {
        public ToolBarDateRunnerPolicyInput(bool hasModalDialog, bool isOptionDialogShowing, bool isContextMenuShowing, bool hasFrameOverlay, bool isMapSelectorShowing)
        {
            this.HasModalDialog = hasModalDialog;
            this.IsOptionDialogShowing = isOptionDialogShowing;
            this.IsContextMenuShowing = isContextMenuShowing;
            this.HasFrameOverlay = hasFrameOverlay;
            this.IsMapSelectorShowing = isMapSelectorShowing;
        }

        public bool HasModalDialog { get; }
        public bool IsOptionDialogShowing { get; }
        public bool IsContextMenuShowing { get; }
        public bool HasFrameOverlay { get; }
        public bool IsMapSelectorShowing { get; }
    }

    public enum ToolBarLockFlow
    {
        None,
        Selector,
        MapViewSelector,
        Dialog
    }

    public readonly struct ToolBarDateRunnerFlowPolicy
    {
        public ToolBarDateRunnerFlowPolicy(bool lockInNoneFlow, bool lockInSelector, bool lockInMapViewSelector, bool lockInDialog)
        {
            this.LockInNoneFlow = lockInNoneFlow;
            this.LockInSelector = lockInSelector;
            this.LockInMapViewSelector = lockInMapViewSelector;
            this.LockInDialog = lockInDialog;
        }

        public bool LockInNoneFlow { get; }
        public bool LockInSelector { get; }
        public bool LockInMapViewSelector { get; }
        public bool LockInDialog { get; }
    }

    public readonly struct ToolBarDateRunnerPolicyDecision
    {
        public ToolBarDateRunnerPolicyDecision(bool suspendDateRunner, bool lockToolBarInput)
        {
            this.SuspendDateRunner = suspendDateRunner;
            this.LockToolBarInput = lockToolBarInput;
        }

        public bool SuspendDateRunner { get; }
        public bool LockToolBarInput { get; }
        public bool AllowDateRunnerProgress => !this.SuspendDateRunner;

        public bool ShouldLockInputForFlow(ToolBarLockFlow flow, ToolBarDateRunnerFlowPolicy flowPolicy)
        {
            if (!this.LockToolBarInput) return false;
            switch (flow)
            {
                case ToolBarLockFlow.Selector:
                    return flowPolicy.LockInSelector;
                case ToolBarLockFlow.MapViewSelector:
                    return flowPolicy.LockInMapViewSelector;
                case ToolBarLockFlow.Dialog:
                    return flowPolicy.LockInDialog;
                default:
                    return flowPolicy.LockInNoneFlow;
            }
        }
    }
}
