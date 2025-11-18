// Minimal Screen base class for Phase 3 UI framework
#pragma once

#include <memory>

namespace WO3K {

class Screen {
public:
    virtual ~Screen() = default;

    // Lifecycle
    virtual void Initialize() {}
    virtual void OnEnter() {}
    virtual void OnExit() {}

    // Main loop callbacks
    virtual void Update(double /*deltaTime*/) {}
    virtual void Draw() {}

    // Window resize notification
    virtual void OnResize(int /*width*/, int /*height*/) {}

    // Event handling: return true if event was handled and should stop propagation.
    // Using void* for generic event payload to avoid dependency on SDL types here.
    virtual bool HandleEvent(const void* /*event*/) { return false; }
};

using ScreenPtr = std::shared_ptr<Screen>;

} // namespace WO3K
