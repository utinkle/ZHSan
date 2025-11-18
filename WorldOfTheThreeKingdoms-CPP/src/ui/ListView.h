#pragma once
#include "ui/Control.h"
#include "ui/ScrollBar.h"
#include <string>
#include <vector>
#include <functional>

namespace WO3K {

class ListView : public Control {
public:
    ListView();

    std::vector<std::string> Items;
    int SelectedIndex = -1;
    int ItemHeight = 28;
    int ScrollOffset = 0; // pixels
    // External scrollbar control instance (owned by ListView)
    std::shared_ptr<ScrollBar> ScrollBarControl;

    // Colors for text
    uint8_t FontR = 230, FontG = 230, FontB = 230;

    // Callback when item is activated (double click or Enter)
    std::function<void(int)> OnItemActivated;
    
    // Scrollbar internal state
    bool ScrollDragging = false;
    int ScrollDragStartY = 0;
    int ScrollStartOffset = 0;


    void Initialize() override;
    void Update(double deltaTime) override;
    void Draw() override;
    bool HandleEvent(const void* event) override;
};

using ListViewPtr = std::shared_ptr<ListView>;

} // namespace WO3K
