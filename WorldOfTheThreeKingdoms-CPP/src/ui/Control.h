#pragma once

#include <functional>
#include <memory>
#include "platform/Types.h"

namespace WO3K {

class Control : public std::enable_shared_from_this<Control> {
public:
    virtual ~Control() = default;

    // Basic properties
    bool Visible = true;
    bool Enabled = true;

    // Bounds in screen coordinates
    Platform::Rectangle Bounds;
    
    // Simple layout docking mode. Default is Dock::None (manual bounds)
    enum class Dock {
        None = 0,
        Top,
        Bottom,
        Left,
        Right,
        Fill
    } DockMode = Dock::None;
    
    // Margin inside docked area
    struct Margin {
        int Left = 0;
        int Top = 0;
        int Right = 0;
        int Bottom = 0;
    } Margin;

    // Whether control can receive focus via keyboard navigation
    bool Focusable = false;

    // Lifecycle
    virtual void Initialize() {}
    virtual void Update(double deltaTime) {}
    virtual void Draw() {}

    // Handle an event (SDL_Event* passed as void*)
    // Return true if handled to stop propagation
    virtual bool HandleEvent(const void* event) { return false; }

    // Focus callbacks
    virtual void OnFocus() {}
    virtual void OnBlur() {}

    // Hit test
    virtual bool ContainsPoint(int x, int y) const {
        return x >= Bounds.X && x < (Bounds.X + Bounds.Width) && y >= Bounds.Y && y < (Bounds.Y + Bounds.Height);
    }
};

using ControlPtr = std::shared_ptr<Control>;

} // namespace WO3K
