#pragma once

#include "screen/Screen.h"
#include "ui/Panel.h"
#include "ui/BoxPanel.h"
#include "platform/Texture.h"
#include "ui/ListView.h"
#include <functional>
#include <vector>

namespace WO3K {

class MainGameScreen : public Screen {
public:
    MainGameScreen() = default;
    ~MainGameScreen() override = default;

    void Initialize() override;
    void Update(double deltaTime) override;
    void Draw() override;
    bool HandleEvent(const void* event) override;
    void OnResize(int width, int height) override;

    // Provide a callback that returns unit names for the HUD/list view.
    void SetUnitProvider(const std::function<std::vector<std::string>()>& provider) { unitProvider = provider; }

private:
    std::shared_ptr<Panel> rootPanel;
    std::shared_ptr<BoxPanel> leftBoxPanel;
    std::shared_ptr<Texture> miniMapTexture;
    std::shared_ptr<WO3K::ListView> unitList;

    // External provider callback to supply unit names for HUD/listing.
    // This allows binding to the game model when GameScenario is implemented.
    std::function<std::vector<std::string>()> unitProvider;

    void RefreshHUD();

    // simulated world / camera sizes for mini-map mapping
    int worldWidth = 2048;
    int worldHeight = 1536;
    int camX = 0, camY = 0, camW = 512, camH = 384;
    // smooth camera target
    int camTargetX = 0, camTargetY = 0;
    float camLerpSpeed = 8.0f; // units per second interpolation multiplier

    bool draggingMiniMap = false;
    // dragging viewport rectangle specifically
    bool draggingViewport = false;
    int viewportDragOffsetX = 0, viewportDragOffsetY = 0;
};

} // namespace WO3K
