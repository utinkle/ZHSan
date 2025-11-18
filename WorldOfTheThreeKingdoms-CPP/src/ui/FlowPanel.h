#pragma once
#include "ui/Panel.h"

namespace WO3K {

class FlowPanel : public Panel {
public:
    enum class Direction { Horizontal, Vertical };
    FlowPanel();

    Direction FlowDirection = Direction::Horizontal;
    int Spacing = 6;
    int Padding = 4;
    // When true, items will wrap to next line/column when overflow occurs
    bool Wrap = false;

    // Alignment for items along the primary axis for each line/column
    enum class Alignment { Start, Center, End };
    Alignment Align = Alignment::Start;

    void Initialize() override;
    void Update(double deltaTime) override;
    void Draw() override;

    // layout pass for flow
    void ApplyFlowLayout();
};

using FlowPanelPtr = std::shared_ptr<FlowPanel>;

} // namespace WO3K
