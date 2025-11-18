#include "screen/ScreenManager.h"

namespace WO3K {

void ScreenManager::Push(const ScreenPtr& screen) {
    if (!screen) return;
    if (!m_stack.empty()) {
        m_stack.back()->OnExit();
    }
    m_stack.push_back(screen);
    screen->OnEnter();
}

void ScreenManager::Pop() {
    if (m_stack.empty()) return;
    auto top = m_stack.back();
    top->OnExit();
    m_stack.pop_back();
    if (!m_stack.empty()) {
        m_stack.back()->OnEnter();
    }
}

void ScreenManager::Replace(const ScreenPtr& screen) {
    if (m_stack.empty()) {
        Push(screen);
        return;
    }
    m_stack.back()->OnExit();
    m_stack.back() = screen;
    if (screen) screen->OnEnter();
}

void ScreenManager::Update(double deltaTime) {
    if (!m_stack.empty() && m_stack.back()) {
        m_stack.back()->Update(deltaTime);
    }
}

void ScreenManager::Draw() {
    if (!m_stack.empty() && m_stack.back()) {
        m_stack.back()->Draw();
    }
}

bool ScreenManager::HandleEvent(const void* event) {
    if (!m_stack.empty() && m_stack.back()) {
        return m_stack.back()->HandleEvent(event);
    }
    return false;
}

void ScreenManager::OnResize(int width, int height) {
    for (auto& s : m_stack) {
        if (s) s->OnResize(width, height);
    }
}

ScreenPtr ScreenManager::GetCurrent() const {
    if (m_stack.empty()) return nullptr;
    return m_stack.back();
}

size_t ScreenManager::GetStackSize() const {
    return m_stack.size();
}

} // namespace WO3K
