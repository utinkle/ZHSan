using ZHSan.Core.Infrastructure.Configuration;

namespace ZHSan.Core.Presentation.UI.Services
{
    public sealed class ToolBarDateRunnerFlowPolicyBuilder
    {
        public ToolBarDateRunnerFlowPolicy Build(RuntimeOptions options)
        {
            var policy = options?.Ui?.ToolBarDateRunnerPolicy;
            if (policy == null)
            {
                return new ToolBarDateRunnerFlowPolicy(true, true, true, true);
            }

            return new ToolBarDateRunnerFlowPolicy(policy.LockInNoneFlow, policy.LockInSelector, policy.LockInMapViewSelector, policy.LockInDialog);
        }
    }
}
