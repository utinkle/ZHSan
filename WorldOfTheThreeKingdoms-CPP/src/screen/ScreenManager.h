#pragma once

#include "screen/Screen.h"
#include <vector>
#include <memory>

namespace WO3K {

class ScreenManager {
public:
    ScreenManager() = default;
    ~ScreenManager() = default;

    // Push a new screen on the stack. The new screen will receive OnEnter().
    void Push(const ScreenPtr& screen);

    // Pop current screen. Calls OnExit() on the popped screen.
    void Pop();

    // Replace the current top screen with a new one.
    void Replace(const ScreenPtr& screen);

    // Update / Draw current top screen
    void Update(double deltaTime);
    void Draw();

    // Notify screens of a window resize
    void OnResize(int width, int height);

    // Dispatch an event to the top-most screen. Returns true if handled.
    bool HandleEvent(const void* event);

    // Accessors
    ScreenPtr GetCurrent() const;
    size_t GetStackSize() const;

private:
    std::vector<ScreenPtr> m_stack;
};

} // namespace WO3K
