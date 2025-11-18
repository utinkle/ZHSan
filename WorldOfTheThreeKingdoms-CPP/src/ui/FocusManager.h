#pragma once

#include "ui/Control.h"
#include <memory>

namespace WO3K {

class FocusManager {
public:
    // Set focus to a control. Passing nullptr clears focus.
    static void SetFocus(const ControlPtr& control);
    static ControlPtr GetFocus();
    static void ClearFocus();

    // Move focus to next control in a container (not implemented yet)
    static void MoveFocusNext();
    static void MoveFocusPrev();

    // Register/unregister focusable controls (called by Panel or control constructors)
    static void Register(const ControlPtr& control);
    static void Unregister(const ControlPtr& control);

private:
    static ControlPtr s_focused;
};

} // namespace WO3K
