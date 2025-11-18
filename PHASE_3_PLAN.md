# Phase 3 — UI / Screens / Gameplay UI Planning

目标：把 C# 的 UI / Screen 层（GameScreens、GamePanels、MainGameScreen、ScreenManager 等）逐步迁移到 C++ / SDL2，同时设计可复用、可测试的屏幕/面板/控件框架，为 Phase 4 的游戏对象和系统提供稳固展示和交互基础。

概览与范围
- 覆盖：Screen/ScreenManager、Panel/Widget（ButtonTexture、Scrollbar、TextBox 等）、MainMenu/Loading/MainGameScreen 层、事件/焦点/输入路由、UI 资源（图集/字体/文本渲染）、与 CacheManager、InputManager、MainGame 的集成。
- 不覆盖：具体游戏对象逻辑（Phase 4 负责），复杂 AI/策略逻辑。

设计要点
- 小而明确的职责：Screen 负责生命周期（OnEnter/OnExit/Update/Draw），ScreenManager 负责屏幕栈和切换。
- 分层渲染：Layer（地图层、单位层、UI 层）支持顺序渲染和可选裁剪/遮罩。
- 轻量控件系统：基于矩形边界检测的 Widget/Control 实现（Button、Scrollbar、TextBox、List、Panel）。
- 输入路由：先由 InputManager 收集原始事件，再传给 ScreenManager → 当前 Screen → 控件（按先后和焦点）处理。
- 布局与缩放：支持基于分辨率的 UI 缩放策略（固定比例或锚点布局）。
- 资源与内容：使用 `CacheManager` 管理纹理/字体、使用 `SpriteFontPlus` 或 SDL_ttf 的替代实现做文本渲染。
- 序列化：保持现有 C# 存档/配置兼容，GameObjects 的序列化由 Phase 4 处理；UI 的可序列化部分仅限于界面布局/用户设置。

主要模块与文件（建议路径）
- `src/screen/Screen.h/.cpp` — 抽象基类，声明生命周期回调（OnInitialize、OnEnter、OnExit、Update、Draw、HandleEvent）。
- `src/screen/ScreenManager.h/.cpp` — 管理屏幕栈，切换动画、模态对话框支持。
- `src/screen/MainMenuScreen.h/.cpp` — 启动/设置/载入菜单。
- `src/screen/LoadingScreen.h/.cpp` — 资源加载进度视图。
- `src/screen/MainGameScreen.h/.cpp` — 将现有 `MainGameScreen` 的渲染层拆分为多个 Layer 对象并移植。
- `src/ui/Control.h/.cpp` — 基本控件接口（Bounds、Visible、Enabled、Focus、Update、Draw、OnEvent）
- `src/ui/Panel.h/.cpp` — 容器控件，管理子控件、布局
- `src/ui/Button.h/.cpp`, `src/ui/Scrollbar.h/.cpp`, `src/ui/TextBox.h/.cpp`, `src/ui/ListView.h/.cpp` — 主要控件
- `src/ui/Layout.h/.cpp` — 简单布局策略（Absolute、Vertical、Horizontal、Anchor）
- `src/ui/Style.h` — 主题/样式（颜色、字体、间距）
- `src/ui/FocusManager.h/.cpp` — 焦点管理（Tab 顺序、鼠标捕获）
- `src/ui/Event.h` — UI 事件类型（Click、DoubleClick、MouseMove、KeyDown）

输入与事件流
1. `PlatformSDL` 把 SDL 事件转成 `InputManager` 的统一事件
2. `MainGame` 在主循环早期分发事件到 `ScreenManager`。
3. `ScreenManager` 先把事件给模态覆盖层（如对话框），否则传递给当前 `Screen`。
4. `Screen` 将事件按控件渲染顺序投递到 `Panel`/`Control`（支持捕获事件和阻止冒泡）。

渲染与 Layer
- 每个 `Screen` 持有一组 `Layer`（例如：BackgroundLayer, MapLayer, TroopLayer, UILayer）。
- Layer 自身只负责 Draw，且可按需开启/关闭/排序。
- `SpriteBatch` 继续用作批量渲染；控件渲染应使用统一的坐标系（屏幕或画布坐标）。

字体与文本渲染
- 方案：继续使用 `SpriteFontPlus` 子模块中的字体烘焙工具，或作为短期方案使用 `SDL_ttf` +纹理缓存。
- 所有文本绘制通过 `TextManager`/`FontManager` 进行，充分利用 `CacheManager` 做纹理化字形缓存。

资源与内容管线
- 保留 `Content/` 文件夹结构，资源引用使用相对路径（例如：`Textures/UI/button.png`）。
- UI 资源打包建议（长期）：提供一个可选的“资源清单”生成脚本把常用纹理列入列表，以便安装或发布时打包。

序列化与设置
- UI 布局（如果需要可编辑）采用 JSON 定义（位置、大小、可见性、文本）。
- 用户设置（分辨率、缩放、音量）继续使用现有 `Setting` 系统。

迁移步骤（迭代式）
1. 设计并实现 `Screen` 与 `ScreenManager` 抽象（接口级别）。
2. 实现 `Control` 基类和 `Panel`，并移植 `ButtonTexture` 为 `ui/Button` 的第一个实现（保留 API 如 `OnClick`、`MouseOver`）。
3. 实现 `FocusManager` 和事件冒泡机制。
4. 移植 `MainMenuScreen`（最小化实现：静态菜单项、开始/设置按钮）。
5. 实现 `LoadingScreen`，并用它包装资源加载逻辑（测试 CacheManager 的加载与缓存）。
6. 将 `MainGameScreen` 的 UI 层拆分并逐步替换为新的控件/Layer 实现（分步骤替换：UI 控件 → 交互 → 渲染）。
7. 调整 `MainGame` 初始化流程来初始化 UI 子系统。

测试与验证策略
- 单元测试：控件边界检测、事件投递、Focus 管理
- 集成测试：启动 MainGame → 打开 MainMenu → 点击按钮触发行为 → 跳转 LoadingScreen → 进入 MainGameScreen
- 可视化回归：保存关键界面截图（不同分辨率）做对比

时间估算（粗略）
- 架构与基础实现（Screen + ScreenManager + Control）：2-3 天
- 主要控件实现（Button, Scrollbar, TextBox, List）：3-5 天
- MainMenu + LoadingScreen 移植与验证：1-2 天
- MainGameScreen UI 层拆分与集成：3-5 天
- 测试与调整：2-3 天

交付物（短期）
- `PHASE_3_PLAN.md`（本文件）
- `src/screen/*` 基础代码样板
- `src/ui/*` 控件骨架
- 集成测试脚本/步骤文档

下一步（我将执行）
1. 在 `d:\project\ZHSan\` 下创建 `src/screen/` 和 `src/ui/` 的文件骨架并提交（若您确认，我会继续创建样板文件并运行一次构建验证）。
2. 如果同意时间估算，我会先实现 `Screen`/`ScreenManager` 和 `Control` 基类。

---

文档结束。