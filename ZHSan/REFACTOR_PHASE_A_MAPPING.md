# Phase A 旧模块 -> 新分层映射（首版）

> 用于完成 `REFACTOR_TODO_EXECUTION.md` 中 A-1 映射项；后续按模块持续补充。

| 旧文件/模块 | 新分层归属 | 说明 |
|---|---|---|
| `ZHSan.Core/MainGame.cs` | Infrastructure + Presentation(入口编排) | 逐步收缩为启动编排层，减少业务逻辑混入 |
| `ZHSan.Core/GameScreens/ScreenManager.cs` | Application | 拆分为 UseCase + Command + Event 发布 |
| `ZHSan.Core/GamePlugins/GamePlugin.cs` | Infrastructure + Presentation | 由硬编码初始化迁移为配置化装配 |
| `ZHSan.Core/GameManager/AnimatedTexture.cs` | Rendering | 作为 LegacyAdapter 过渡至新动画系统 |
| `ZHSan.Core/GameScreens/MainGameScreen.cs` | Presentation | UI/交互编排层，减少对 Domain 直接操作 |

