#pragma once
#include "ui/Control.h"
#include <functional>

namespace WO3K {

class ScrollBar : public Control {
public:
    enum class Orientation { Vertical };
    ScrollBar();

    Orientation orient = Orientation::Vertical;
    int Min = 0;
    int Max = 0;
    int PageSize = 0; // visible size
    int Value = 0; // current scroll offset

    std::function<void(int)> OnValueChanged;

    void Initialize() override;
    void Update(double deltaTime) override;
    void Draw() override;
    bool HandleEvent(const void* event) override;

    // Dragging state
    bool Dragging = false;
    int DragStartY = 0;
    int DragStartValue = 0;

    void SetRange(int minv, int maxv, int page);
    void SetValue(int v);
};

using ScrollBarPtr = std::shared_ptr<ScrollBar>;

} // namespace WO3K
