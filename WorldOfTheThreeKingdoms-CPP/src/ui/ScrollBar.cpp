#include "ui/ScrollBar.h"
#include "platform/PlatformSDL.h"
#include <SDL.h>
#include <algorithm>

namespace WO3K {

ScrollBar::ScrollBar() {
    Focusable = false;
}

void ScrollBar::Initialize() {}

void ScrollBar::Update(double /*deltaTime*/) {
    if (Max < Min) Max = Min;
    if (Value < Min) Value = Min;
    if (Value > Max) Value = Max;
}

void ScrollBar::SetRange(int minv, int maxv, int page) {
    Min = minv; Max = std::max(minv, maxv); PageSize = page;
    if (Value < Min) Value = Min;
    if (Value > Max) Value = Max;
}

void ScrollBar::SetValue(int v) {
    int nv = std::max(Min, std::min(Max, v));
    if (nv != Value) {
        Value = nv;
        if (OnValueChanged) OnValueChanged(Value);
    }
}

void ScrollBar::Draw() {
    SDL_Renderer* renderer = Platform::GetSDLRenderer();
    if (!renderer) return;

    int contentH = Max - Min + PageSize;
    if (contentH <= PageSize) return; // nothing to draw

    int trackX = Bounds.X;
    int trackY = Bounds.Y;
    int trackW = Bounds.Width;
    int trackH = Bounds.Height;

    SDL_Rect track{ trackX, trackY, trackW, trackH };
    SDL_SetRenderDrawColor(renderer, 24, 24, 32, 200);
    SDL_RenderFillRect(renderer, &track);

    float ratio = (float)PageSize / (float)contentH;
    int thumbH = std::max(12, (int)(trackH * ratio));
    float scrollRatio = (float)(Value - Min) / (float)std::max(1, Max - Min);
    int thumbY = trackY + (int)((trackH - thumbH) * scrollRatio);
    SDL_Rect thumb{ trackX, thumbY, trackW, thumbH };
    SDL_SetRenderDrawColor(renderer, 120, 120, 140, 255);
    SDL_RenderFillRect(renderer, &thumb);
}

bool ScrollBar::HandleEvent(const void* event) {
    if (!event || !Visible || !Enabled) return false;
    const SDL_Event* ev = static_cast<const SDL_Event*>(event);
    if (!ev) return false;

    switch (ev->type) {
    case SDL_MOUSEBUTTONDOWN:
        if (ev->button.button == SDL_BUTTON_LEFT) {
            int mx = ev->button.x, my = ev->button.y;
            if (mx >= Bounds.X && mx <= Bounds.X + Bounds.Width && my >= Bounds.Y && my <= Bounds.Y + Bounds.Height) {
                int trackH = Bounds.Height;
                int contentH = Max - Min + PageSize;
                float ratio = (float)PageSize / (float)contentH;
                int thumbH = std::max(12, (int)(trackH * ratio));
                int thumbY = Bounds.Y + (int)((trackH - thumbH) * ((float)(Value - Min) / std::max(1, Max - Min)));
                SDL_Rect thumb{ Bounds.X, thumbY, Bounds.Width, thumbH };
                if (my >= thumb.y && my <= thumb.y + thumb.h) {
                    // begin dragging: capture mouse and store start state
                    Dragging = true;
                    DragStartY = my;
                    DragStartValue = Value;
                    SDL_CaptureMouse(SDL_TRUE);
                    return true;
                } else {
                    // click on track: page up/down
                    if (my < thumbY) SetValue(Value - PageSize);
                    else SetValue(Value + PageSize);
                    return true;
                }
            }
        }
        break;
    case SDL_MOUSEWHEEL:
    {
        int mx=0,my=0;
        SDL_GetMouseState(&mx,&my);
        if (mx >= Bounds.X && mx <= Bounds.X + Bounds.Width && my >= Bounds.Y && my <= Bounds.Y + Bounds.Height) {
            SetValue(Value - ev->wheel.y * PageSize / 3);
            return true;
        }
    }
        break;
    case SDL_MOUSEMOTION:
        if (Dragging) {
            int my = ev->motion.y;
            int trackH = Bounds.Height;
            int contentH = Max - Min + PageSize;
            int thumbH = std::max(12, (int)(trackH * ((float)PageSize / (float)std::max(1, contentH))));
            int movable = std::max(1, trackH - thumbH);
            int dy = my - DragStartY;
            float deltaRatio = (float)dy / (float)movable;
            int maxv = std::max(0, Max - Min);
            int newVal = DragStartValue + (int)(deltaRatio * maxv);
            SetValue(newVal);
            return true;
        }
        break;
    case SDL_MOUSEBUTTONUP:
        if (ev->button.button == SDL_BUTTON_LEFT) {
            if (Dragging) {
                Dragging = false;
                SDL_CaptureMouse(SDL_FALSE);
                return true;
            }
        }
        break;
    default:
        break;
    }
    return false;
}

} // namespace WO3K
