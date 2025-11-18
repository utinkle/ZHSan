#include "ui/ListView.h"
#include "ui/FontManager.h"
#include "platform/PlatformSDL.h"
#include "ui/FocusManager.h"
#include "audio/SoundManager.h"
#include "ui/ScrollBar.h"
#include <SDL.h>
#include <algorithm>

namespace WO3K {

ListView::ListView() {
    Focusable = true;
}

void ListView::Initialize() {
    // create scrollbar control instance
    ScrollBarControl = std::make_shared<ScrollBar>();
    ScrollBarControl->orient = ScrollBar::Orientation::Vertical;
    ScrollBarControl->Bounds = { Bounds.X + Bounds.Width - 12, Bounds.Y, 12, Bounds.Height };
    ScrollBarControl->Initialize();
    // wire value changed
    ScrollBarControl->OnValueChanged = [this](int v) {
        ScrollOffset = v;
    };
}

void ListView::Update(double deltaTime) {
    // clamp scroll offset
    int contentH = (int)Items.size() * ItemHeight;
    int maxOffset = std::max(0, contentH - Bounds.Height);
    if (ScrollOffset < 0) ScrollOffset = 0;
    if (ScrollOffset > maxOffset) ScrollOffset = maxOffset;

    // update scrollbar range and position
    if (ScrollBarControl) {
        ScrollBarControl->Bounds = { Bounds.X + Bounds.Width - 12, Bounds.Y, 12, Bounds.Height };
        ScrollBarControl->SetRange(0, maxOffset, Bounds.Height);
        ScrollBarControl->SetValue(ScrollOffset);
        ScrollBarControl->Update(deltaTime);
    }
}

void ListView::Draw() {
    SDL_Renderer* renderer = Platform::GetSDLRenderer();
    if (!renderer) return;

    // background
    SDL_SetRenderDrawColor(renderer, 30, 30, 40, 255);
    SDL_Rect bg{ Bounds.X, Bounds.Y, Bounds.Width, Bounds.Height };
    SDL_RenderFillRect(renderer, &bg);

    // draw items (apply scroll offset)
    int y = Bounds.Y - ScrollOffset;
    int maxVisible = Bounds.Height / ItemHeight;
    for (size_t i = 0; i < Items.size(); ++i) {
        if (y + ItemHeight < Bounds.Y) { y += ItemHeight; continue; }
        if (y > Bounds.Y + Bounds.Height) break;

        SDL_Rect itrect{ Bounds.X, y, Bounds.Width, ItemHeight };
        if ((int)i == SelectedIndex) {
            SDL_SetRenderDrawColor(renderer, 70, 70, 100, 255);
            SDL_RenderFillRect(renderer, &itrect);
        } else {
            // subtle striping
            if (i % 2 == 0) SDL_SetRenderDrawColor(renderer, 34, 34, 44, 255);
            else SDL_SetRenderDrawColor(renderer, 32, 32, 40, 255);
            SDL_RenderFillRect(renderer, &itrect);
        }

        // draw text
        if (!Items[i].empty()) {
            auto ttex = FontManager::RenderText("Content/Font/SimHei.ttf:16", Items[i], FontR, FontG, FontB);
            if (ttex && ttex->GetSDLTexture()) {
                int w = ttex->width();
                int h = ttex->height();
                SDL_Rect dst{ Bounds.X + 6, y + (ItemHeight - h) / 2, std::min(w, Bounds.Width - 12), h };
                SDL_RenderCopy(renderer, ttex->GetSDLTexture(), nullptr, &dst);
            }
        }

        y += ItemHeight;
    }

    // draw scrollbar via ScrollBarControl
    if (ScrollBarControl) ScrollBarControl->Draw();
}

bool ListView::HandleEvent(const void* event) {
    if (!event || !Visible || !Enabled) return false;
    const SDL_Event* ev = static_cast<const SDL_Event*>(event);
    if (!ev) return false;

    // let scrollbar handle event first
    if (ScrollBarControl && ScrollBarControl->HandleEvent(event)) {
        // sync offset from scrollbar
        ScrollOffset = ScrollBarControl->Value;
        return true;
    }

    switch (ev->type) {
    case SDL_MOUSEBUTTONDOWN:
        if (ev->button.button == SDL_BUTTON_LEFT) {
            int x = ev->button.x;
            int y = ev->button.y;
            if (ContainsPoint(x, y)) {
                int idx = (y - Bounds.Y + ScrollOffset) / ItemHeight;
                if (idx >= 0 && idx < (int)Items.size()) {
                    SelectedIndex = idx;
                    try { FocusManager::SetFocus(shared_from_this()); } catch(...) {}
                    // click sound
                    WO3K::SoundManager::Play("button_click.wav");
                    return true;
                }
            }
        }
        break;
    case SDL_MOUSEMOTION:
        // if dragging scrollbar, update offset
        if (ScrollDragging) {
            int my = ev->motion.y;
            int dy = my - ScrollDragStartY;
            int contentH = (int)Items.size() * ItemHeight;
            int trackH = Bounds.Height;
            int maxOffset = std::max(0, contentH - Bounds.Height);
            if (maxOffset > 0) {
                float movable = (float)(trackH - std::max(20, (int)(trackH * ((float)Bounds.Height / (float)contentH))));
                if (movable > 0) {
                    float deltaRatio = (float)dy / movable;
                    int newOffset = ScrollStartOffset + (int)(deltaRatio * maxOffset);
                    ScrollOffset = std::max(0, std::min(maxOffset, newOffset));
                }
            }
            return true;
        }
        // highlight on hover but don't change selection
        break;
    case SDL_MOUSEBUTTONUP:
        if (ev->button.button == SDL_BUTTON_LEFT) {
            if (ScrollDragging) { ScrollDragging = false; return true; }
        }
        break;
    case SDL_MOUSEWHEEL:
        // mouse wheel scroll anywhere on list
        ScrollOffset -= ev->wheel.y * ItemHeight;
        return true;
        break;
    case SDL_KEYDOWN:
        if (ev->key.keysym.sym == SDLK_DOWN) {
            if (SelectedIndex < (int)Items.size() - 1) SelectedIndex++;
            else if (SelectedIndex == -1 && !Items.empty()) SelectedIndex = 0;
            return true;
        } else if (ev->key.keysym.sym == SDLK_UP) {
            if (SelectedIndex > 0) SelectedIndex--;
            return true;
        } else if (ev->key.keysym.sym == SDLK_RETURN || ev->key.keysym.sym == SDLK_SPACE) {
            if (SelectedIndex >= 0 && SelectedIndex < (int)Items.size()) {
                if (OnItemActivated) OnItemActivated(SelectedIndex);
                WO3K::SoundManager::Play("button_click.wav");
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
