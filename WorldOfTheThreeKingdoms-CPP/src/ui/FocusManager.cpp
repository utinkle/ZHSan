#include "ui/FocusManager.h"
#include <iostream>

namespace WO3K {

ControlPtr FocusManager::s_focused = nullptr;

// Registry of focusable controls (weak pointers to avoid ownership cycles)
std::vector<std::weak_ptr<Control>> g_focusRegistry;
size_t g_focusIndex = 0;

void FocusManager::SetFocus(const ControlPtr& control) {
    if (s_focused == control) return;
    if (s_focused) s_focused->OnBlur();
    s_focused = control;
    // Update registry index to point to currently focused control
    if (control) {
        // find control in registry
        for (size_t i = 0; i < g_focusRegistry.size(); ++i) {
            if (auto p = g_focusRegistry[i].lock()) {
                if (p == control) { g_focusIndex = i; break; }
            }
        }
        s_focused->OnFocus();
    }
    if (control) {
        std::cout << "FocusManager: Set focus on control\n";
    } else {
        std::cout << "FocusManager: Cleared focus\n";
    }
}

ControlPtr FocusManager::GetFocus() {
    return s_focused;
}

void FocusManager::ClearFocus() {
    s_focused = nullptr;
}

void FocusManager::MoveFocusNext() {
    // Advance to next registered focusable control
    if (g_focusRegistry.empty()) return;

    size_t start = g_focusIndex;
    for (size_t i = 1; i <= g_focusRegistry.size(); ++i) {
        size_t idx = (start + i) % g_focusRegistry.size();
        if (auto p = g_focusRegistry[idx].lock()) {
            SetFocus(p);
            g_focusIndex = idx;
            return;
        }
    }
}

void FocusManager::MoveFocusPrev() {
    if (g_focusRegistry.empty()) return;
    size_t start = g_focusIndex;
    for (size_t iter = 1; iter <= g_focusRegistry.size(); ++iter) {
        size_t idx = (start + g_focusRegistry.size() - iter) % g_focusRegistry.size();
        if (auto p = g_focusRegistry[idx].lock()) {
            SetFocus(p);
            g_focusIndex = idx;
            return;
        }
    }
}

void FocusManager::Register(const ControlPtr& control) {
    if (!control) return;
    // avoid duplicate registration
    for (auto &w : g_focusRegistry) {
        if (auto p = w.lock()) if (p == control) return;
    }
    g_focusRegistry.push_back(control);
}

void FocusManager::Unregister(const ControlPtr& control) {
    if (!control) return;
    for (auto it = g_focusRegistry.begin(); it != g_focusRegistry.end(); ) {
        if (auto p = it->lock()) {
            if (p == control) it = g_focusRegistry.erase(it);
            else ++it;
        } else {
            it = g_focusRegistry.erase(it);
        }
    }
}

} // namespace WO3K
