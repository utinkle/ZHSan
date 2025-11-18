#pragma once
#include "ui/Panel.h"
#include <string>

namespace WO3K {

class BoxPanel : public Panel {
public:
    BoxPanel() = default;
    ~BoxPanel() override = default;

    // background color
    uint8_t R = 40, G = 40, B = 48, A = 255;
    bool DrawBorder = false;

    // Optional title text displayed in a title bar at top
    std::string Title;
    int TitleHeight = 22;

    // Title bar background color
    uint8_t TitleR = 30, TitleG = 30, TitleB = 36, TitleA = 255;
    
    // Rounded corner radius (0 = no rounding)
    int RoundedRadius = 0;

    // Drop shadow options
    bool DrawShadow = false;
    int ShadowOffsetX = 4;
    int ShadowOffsetY = 4;
    uint8_t ShadowR = 0, ShadowG = 0, ShadowB = 0, ShadowA = 120;

    // Interactive viewport handle visual
    bool HandleVisible = false;
    int HandleSize = 12;
    int HandleInset = 6;

    void Draw() override;
};

using BoxPanelPtr = std::shared_ptr<BoxPanel>;

} // namespace WO3K
