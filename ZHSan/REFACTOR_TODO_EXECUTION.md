# ZHSan 重构执行 TODO（按阶段有序推进）

> 基于 `REFACTOR_PLAN_MONOGAME.md` 与当前代码结构制定。  
> 原则：**先功能、后测试**；开发阶段不强制单元测试；所有任务以**高内聚、低耦合**为验收重点。

---

## 当前进行中任务（实时进度）

- 当前任务：**Phase A / A-2 依赖装配与事件骨架**
- 当前进度：
  - [x] 已完成 `ServiceRegistry` 轻量服务注册器落地。
  - [x] 已完成 `IEventBus + SimpleEventBus` 事件总线骨架落地。
  - [x] 已完成 `FeatureFlags` 与 `RuntimeBootstrap` 初始化接入（默认关闭新系统）。
  - [ ] 下一步继续推进 A-1（分层映射表与跨层依赖规则文档化到代码目录结构）。

---

## 0. 标注说明（所有 TODO 均使用以下标签）

- `[P0]`：最高优先级，阻塞后续核心开发。
- `[P1]`：高优先级，应在当前里程碑内完成。
- `[P2]`：中优先级，可并行推进。
- `[ARCH]`：架构与边界任务（解耦、模块化）。
- `[UI]`：Myra UI 迁移任务。
- `[ANIM]`：动画系统（帧动画/Tween/Timeline/粒子）。
- `[CAM]`：摄像机与镜头特效任务。
- `[INFRA]`：基础设施（资源、配置、日志、存档、工具）。
- `[NO-TEST]`：本阶段不要求单元测试，以功能实现为主。
- `[HC-LC]`：明确要求高内聚低耦合。
- `[RISK]`：高风险任务，必须有回滚策略。
- `[DONE-CRITERIA]`：完成定义（功能级）。

---

## 1. 总体节奏（顺序执行）

1. Phase A：重构地基（边界、目录、装配机制）
2. Phase B：UI 基座先行（Myra Runtime + 主题 + 导航）
3. Phase C：核心 UI 插件迁移（从低风险到高风险）
4. Phase D：动画系统升级（Tween/Timeline/Particle）
5. Phase E：摄像机系统重构（跟随 + 震屏 + 红屏 + 镜头粒子）
6. Phase F：流程解耦与收口（ScreenManager/插件硬编码治理）
7. Phase G：功能回归与发布准备（非单测导向）

---

## 2. Phase A — 重构地基（第 1~2 周）

### A-1 模块边界落地
- [ ] `[P0][ARCH][HC-LC][NO-TEST]` 建立命名空间分层：`Domain / Application / Presentation / Rendering / Infrastructure`。
- [ ] `[P0][ARCH][HC-LC]` 明确跨层依赖规则：仅允许上层依赖下层抽象，不反向引用具体实现。
- [ ] `[P1][ARCH]` 输出“旧类 -> 新归属层”映射表（MainGame/ScreenManager/GamePlugin/AnimatedTexture 等）。
- [x] `[P0][RISK][ARCH]` 设计并接入 Feature Flag（支持新旧系统切换）。

`[DONE-CRITERIA]`
- 新目录/命名空间结构可编译。
- 至少完成 1 个核心路径（启动 -> 主界面）的分层调用链。

### A-2 依赖装配与事件骨架
- [x] `[P0][ARCH][HC-LC][NO-TEST]` 引入轻量依赖注入容器（或自建 ServiceRegistry），移除“到处 new”。
- [x] `[P1][ARCH][HC-LC]` 增加 EventBus 骨架（DomainEvent / UiEvent 两类）。
- [ ] `[P1][INFRA]` 增加统一配置入口（图形、UI、特效、输入、调试开关）。

`[DONE-CRITERIA]`
- 插件/服务可通过容器解析。
- 业务层可发布事件，表现层可订阅。

---

## 3. Phase B — UI 基座（第 2~4 周）

### B-1 Myra 接入与运行层
- [ ] `[P0][UI][HC-LC][NO-TEST]` 接入 Myra，建立 `MyraUiRuntime`（初始化、Update、Draw、输入桥接）。
- [ ] `[P0][UI][HC-LC]` 建立 `IMyraViewAdapter` 标准接口：`Show/Hide/Bind/Refresh/Dispose`。
- [ ] `[P1][UI]` 建立 UI 层级管理器（Modal、HUD、Overlay、Tooltip 分层）。

`[DONE-CRITERIA]`
- 可以在主循环中稳定绘制 Myra 界面。
- 至少 1 个弹窗可通过 Adapter 生命周期完整运行。

### B-2 主题与导航体系
- [ ] `[P1][UI][HC-LC][NO-TEST]` 建立 `UiStyleTokens`（色板、字号、间距、边框、阴影、动效时长）。
- [ ] `[P1][UI]` 建立 `UiThemeService`（统一换肤入口，支持后续夜间模式/高清缩放）。
- [ ] `[P1][UI]` 建立 `UiNavigationService`（键鼠 + 手柄焦点导航）。

`[DONE-CRITERIA]`
- 所有新 Myra 页面统一样式。
- 焦点在弹窗/列表中可正确循环与确认取消。

---

## 4. Phase C — UI 插件迁移（第 4~8 周）

> 顺序：低风险（通用弹窗）-> 中风险（列表/详情）-> 高风险（复杂组合面板）

### C-1 低风险批次
- [ ] `[P0][UI][NO-TEST]` 迁移 ConfirmationDialog。
- [ ] `[P0][UI][NO-TEST]` 迁移 OptionDialog。
- [ ] `[P1][UI][NO-TEST]` 迁移 Help/Dialog 类静态信息面板。

### C-2 中风险批次
- [ ] `[P0][UI][HC-LC]` 迁移 ContextMenu（命令绑定改为 Command Descriptor，而非直接业务调用）。
- [ ] `[P1][UI]` 迁移 TabList（统一列定义、排序、筛选、分页状态）。
- [ ] `[P1][UI]` 迁移 Person/Troop/Architecture/Treasure Detail 面板。

### C-3 高风险批次
- [ ] `[P0][UI][RISK][HC-LC]` 迁移 ToolBar + DateRunner 交互组合。
- [ ] `[P1][UI][RISK]` 迁移 youcelan/BianduiLiebiao 等历史列表插件。
- [ ] `[P1][UI]` 迁移 MapLayerSelector/Record 等系统面板。

`[DONE-CRITERIA]`
- 旧插件可按开关逐个替换。
- 核心交互（菜单命令、列表选择、详情打开）功能等价。

---

## 5. Phase D — 动画系统升级（第 6~10 周，可与 C 后半并行）

### D-1 动画核心骨架
- [ ] `[P0][ANIM][ARCH][HC-LC][NO-TEST]` 设计 `IAnimationPlayer`、`AnimationClip`、`AnimationTrack` 抽象。
- [ ] `[P0][ANIM]` 实现 Tween 核心（Position/Scale/Rotation/Alpha/Color + Easing）。
- [ ] `[P1][ANIM]` 实现 Timeline（并行/串行/延迟/回调节点）。

### D-2 兼容与替换
- [ ] `[P0][ANIM][RISK]` 提供 `LegacySpriteAnimationAdapter` 兼容 `AnimatedTexture`。
- [ ] `[P1][ANIM]` 将至少 2 条旧特效链替换到新动画系统（例如战斗命中 + UI 提示）。

### D-3 粒子系统
- [ ] `[P0][ANIM][NO-TEST]` 实现 `ParticleEmitter`（生命周期、发射率、初速度、阻尼、颜色曲线）。
- [ ] `[P1][ANIM]` 建立粒子预设（命中火花/烟尘/技能释放/UI 点击反馈）。
- [ ] `[P1][INFRA]` 支持粒子配置数据化（JSON/表驱动）。

`[DONE-CRITERIA]`
- 动画可由逻辑事件触发，不在业务代码中硬编码逐帧细节。
- 粒子可通过参数调整而无需改动核心代码。

---

## 6. Phase E — 摄像机系统重构（第 8~11 周）

### E-1 Camera 核心
- [ ] `[P0][CAM][ARCH][HC-LC][NO-TEST]` 实现 `CameraState`（位置/缩放/旋转/边界约束）。
- [ ] `[P0][CAM]` 实现 `CameraController`（软跟随、硬锁定、目标切换平滑）。
- [ ] `[P1][CAM]` 实现多目标包围盒自动缩放。

### E-2 镜头特效
- [ ] `[P0][CAM]` 实现震屏（强度、频率、衰减、叠加策略）。
- [ ] `[P0][CAM]` 实现红屏（伤害反馈颜色/透明度曲线/时长）。
- [ ] `[P1][CAM][ANIM]` 实现镜头空间粒子叠加（与 Particle 系统联动）。

### E-3 事件联动
- [ ] `[P1][CAM][ARCH]` 将战斗、剧情、灾害事件接入 CameraEffectStack。
- [ ] `[P1][CAM]` 增加“特效优先级与互斥规则”（防止叠加失控）。

`[DONE-CRITERIA]`
- 跟随平滑，无明显抖动。
- 震屏/红屏/镜头粒子均可配置开关与参数。

---

## 7. Phase F — 流程解耦与历史负债治理（第 10~14 周）

### F-1 ScreenManager 解耦
- [ ] `[P0][ARCH][HC-LC][RISK][NO-TEST]` 将 ScreenManager 大流程拆为 UseCase（内政/军事/外交）。
- [ ] `[P1][ARCH]` 引入 Command 对象替换“UI 直接操作业务对象”的调用方式。
- [ ] `[P1][ARCH]` 用 EventBus 连接业务结果与 UI 刷新。

### F-2 插件装配去硬编码
- [ ] `[P0][ARCH][INFRA][HC-LC]` 将 `GamePlugin.InitializePlugins` 改为配置化装配（注册表/清单）。
- [ ] `[P1][INFRA]` 插件生命周期统一（Init/Activate/Deactivate/Dispose）。

### F-3 资源与存档治理
- [ ] `[P1][INFRA][NO-TEST]` 资源加载统一入口（缓存、释放策略、引用计数）。
- [ ] `[P1][INFRA][RISK]` 存档结构版本化 + 迁移器（保证旧存档可读）。

`[DONE-CRITERIA]`
- 关键流程不再依赖巨型类单点分发。
- 新增/替换插件不需要改核心初始化硬编码。

---

## 8. Phase G — 发布前功能回归（第 14~16 周）

> 说明：根据你的要求，开发阶段不强制单元测试；本阶段也以“功能可用性与稳定性验证”为主。

- [ ] `[P0][NO-TEST]` 完成核心玩法全流程人工回归清单（开局->内政->战斗->存读档->结束回合）。
- [ ] `[P0][INFRA]` 完成性能基线复测（FPS、卡顿点、内存峰值）。
- [ ] `[P1][UI][ANIM][CAM]` 完成特效质量档（低/中/高）与默认参数包。
- [ ] `[P1][INFRA]` 编写迁移说明（面向开发者 + 面向策划配置）。

`[DONE-CRITERIA]`
- 核心功能闭环可稳定游玩。
- 新旧存档兼容验证通过。

---

## 9. 关键“禁止事项”（保证高内聚低耦合）

- [ ] `[P0][HC-LC]` 禁止 UI 组件直接操作 Domain 实体内部状态（必须走 UseCase/Command）。
- [ ] `[P0][HC-LC]` 禁止在业务类中直接调用具体 UI 控件 API。
- [ ] `[P0][HC-LC]` 禁止新增全局静态可变状态（除必要只读配置）。
- [ ] `[P0][HC-LC]` 禁止新增“超大类”（>1500 行建议强制拆分）。
- [ ] `[P1][ARCH]` 新功能必须先定义接口与数据契约，再落实现细节。

---

## 10. 每周执行模板（建议落地方式）

每周至少维护以下看板字段：
- `任务编号`（如 A-1、C-2）
- `负责人`
- `状态`（Todo / Doing / Blocked / Done）
- `风险等级`（Low/Medium/High）
- `依赖项`
- `回滚方案`
- `功能验收记录`

建议每周节奏：
1. 周一：锁定本周 P0/P1；
2. 周三：中期风险复盘（重点看 RISK 标签）；
3. 周五：功能验收 + 下周依赖梳理。

---

## 11. 你关心的保证（明确写入执行标准）

- **保证 1：开发阶段不要求实现单元测试**  
  执行方式：所有任务默认 `[NO-TEST]`，优先完成功能闭环与架构落地。

- **保证 2：优先实现功能**  
  执行方式：按 P0/P1 顺序推进，可运行、可交互、可替换优先于“技术完美”。

- **保证 3：高内聚低耦合**  
  执行方式：所有核心任务均附 `[HC-LC]` 标注，并在“禁止事项”中强制约束。

- **保证 4：设计全面性**  
  执行方式：覆盖 UI、动画、摄像机、流程解耦、资源、存档、发布准备全链路。

---

## 12. 下一步（文档阶段）

- [ ] 先把本 TODO 任务拆成项目管理工具可导入格式（CSV/Markdown Checklist）。
- [ ] 再补一份“旧代码文件到任务编号映射表”（例如 `MainGame.cs -> A-1/A-2/F-2`）。
- [ ] 最后进入实现阶段，按 Phase 顺序执行并周更状态。

