#pragma once

#include "ui/Control.h"
#include <vector>

namespace WO3K {

class Panel : public Control {
public:
    Panel() = default;
    ~Panel() override = default;

    void AddChild(const ControlPtr& child) { if (child) children.push_back(child); }
    void RemoveChild(const ControlPtr& child);

    void Initialize() override;
    void Update(double deltaTime) override;
    void Draw() override;
    bool HandleEvent(const void* event) override;

    // Recompute layout for docked children. Call on resize.
    void ApplyDockLayout();

protected:
    std::vector<ControlPtr> children;
};

} // namespace WO3K
