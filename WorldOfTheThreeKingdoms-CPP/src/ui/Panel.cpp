#include "ui/Panel.h"
#include "ui/FocusManager.h"
#include <SDL.h>
#include <algorithm>

namespace WO3K {

void Panel::RemoveChild(const ControlPtr& child) {
    children.erase(std::remove(children.begin(), children.end(), child), children.end());
}

void Panel::Initialize() {
    for (auto& c : children) if (c) {
        c->Initialize();
        // register focusable controls
        if (c->Focusable) {
            try { WO3K::FocusManager::Register(c); } catch(...) {}
        }
    }
    // compute initial dock layout
    ApplyDockLayout();
}

void Panel::ApplyDockLayout() {
    Platform::Rectangle avail = Bounds;
    int left = avail.X;
    int top = avail.Y;
    int right = avail.X + avail.Width;
    int bottom = avail.Y + avail.Height;

    // First pass: Top/Bottom/Left/Right
    for (auto& c : children) if (c) {
        if (c->DockMode == Control::Dock::Top) {
            int h = c->Bounds.Height > 0 ? c->Bounds.Height : 32;
            c->Bounds.X = left + c->Margin.Left;
            c->Bounds.Y = top + c->Margin.Top;
            c->Bounds.Width = (right - left) - c->Margin.Left - c->Margin.Right;
            c->Bounds.Height = h;
            top += h + c->Margin.Top + c->Margin.Bottom;
        } else if (c->DockMode == Control::Dock::Bottom) {
            int h = c->Bounds.Height > 0 ? c->Bounds.Height : 32;
            c->Bounds.X = left + c->Margin.Left;
            c->Bounds.Y = bottom - h - c->Margin.Bottom;
            c->Bounds.Width = (right - left) - c->Margin.Left - c->Margin.Right;
            c->Bounds.Height = h;
            bottom -= h + c->Margin.Top + c->Margin.Bottom;
        } else if (c->DockMode == Control::Dock::Left) {
            int w = c->Bounds.Width > 0 ? c->Bounds.Width : 128;
            c->Bounds.X = left + c->Margin.Left;
            c->Bounds.Y = top + c->Margin.Top;
            c->Bounds.Width = w;
            c->Bounds.Height = (bottom - top) - c->Margin.Top - c->Margin.Bottom;
            left += w + c->Margin.Left + c->Margin.Right;
        } else if (c->DockMode == Control::Dock::Right) {
            int w = c->Bounds.Width > 0 ? c->Bounds.Width : 128;
            c->Bounds.X = right - w - c->Margin.Right;
            c->Bounds.Y = top + c->Margin.Top;
            c->Bounds.Width = w;
            c->Bounds.Height = (bottom - top) - c->Margin.Top - c->Margin.Bottom;
            right -= w + c->Margin.Left + c->Margin.Right;
        }
    }

    // Second pass: Fill
    for (auto& c : children) if (c) {
        if (c->DockMode == Control::Dock::Fill) {
            c->Bounds.X = left + c->Margin.Left;
            c->Bounds.Y = top + c->Margin.Top;
            c->Bounds.Width = (right - left) - c->Margin.Left - c->Margin.Right;
            c->Bounds.Height = (bottom - top) - c->Margin.Top - c->Margin.Bottom;
        }
    }
}

void Panel::Update(double deltaTime) {
    for (auto& c : children) if (c && c->Visible) c->Update(deltaTime);
}

void Panel::Draw() {
    for (auto& c : children) if (c && c->Visible) c->Draw();
}

bool Panel::HandleEvent(const void* event) {
    // If this is a keyboard event, deliver it directly to the focused control first
    const SDL_Event* ev = static_cast<const SDL_Event*>(event);
    if (ev) {
        if (ev->type == SDL_KEYDOWN || ev->type == SDL_KEYUP) {
            auto focused = WO3K::FocusManager::GetFocus();
            if (focused && focused->Visible && focused->Enabled) {
                if (focused->HandleEvent(event)) return true;
            }
        }
    }

    // Dispatch to children in reverse order (top-most first)
    for (auto it = children.rbegin(); it != children.rend(); ++it) {
        auto& c = *it;
        if (c && c->Visible && c->Enabled) {
            if (c->HandleEvent(event)) return true;
        }
    }
    return false;
}

} // namespace WO3K
