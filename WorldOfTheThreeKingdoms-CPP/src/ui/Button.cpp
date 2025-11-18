#include "ui/Button.h"
#include "render/SpriteBatch.h"
#include "platform/PlatformSDL.h"
#include "platform/Texture.h"
#include "ui/FocusManager.h"
#include "ui/FontManager.h"
#include "audio/SoundManager.h"
#include "game/CacheManager.h"
#include <SDL.h>
#include <iostream>
#include <algorithm>

namespace WO3K {

void Button::Update(double deltaTime) {
    // animate hover and press progress toward target values
    float dt = static_cast<float>(deltaTime);
    float targetHover = isHovered ? 1.0f : 0.0f;
    if (hoverProgress < targetHover) hoverProgress = std::min(1.0f, hoverProgress + hoverSpeed * dt);
    else if (hoverProgress > targetHover) hoverProgress = std::max(0.0f, hoverProgress - hoverSpeed * dt);

    float targetPress = isPressed ? 1.0f : 0.0f;
    if (pressProgress < targetPress) pressProgress = std::min(1.0f, pressProgress + pressSpeed * dt);
    else if (pressProgress > targetPress) pressProgress = std::max(0.0f, pressProgress - pressSpeed * dt);
}

void Button::Draw() {
    SDL_Renderer* renderer = Platform::GetSDLRenderer();
    if (!renderer) return;

    // Draw background texture if specified
    // choose texture based on state: pressed -> pressedTextureKey, hovered -> hoverTextureKey, else bgTextureKey
    std::string chosenKey;
    if (isPressed && !pressedTextureKey.empty()) chosenKey = pressedTextureKey;
    else if (isHovered && !hoverTextureKey.empty()) chosenKey = hoverTextureKey;
    else chosenKey = bgTextureKey;

    std::shared_ptr<Texture> chosenTex = nullptr;
    if (!chosenKey.empty()) {
        chosenTex = GameManager::CacheManager::LoadTexture(chosenKey, GameManager::CacheType::Live);
    }

    if (chosenTex && chosenTex->GetSDLTexture()) {
        SDL_Rect dst{ Bounds.X, Bounds.Y, Bounds.Width, Bounds.Height };
        SDL_RenderCopy(renderer, chosenTex->GetSDLTexture(), nullptr, &dst);
    } else {
        // fallback: draw simple colored rect with different shades
        if (isPressed) SDL_SetRenderDrawColor(renderer, 80, 80, 100, 255);
        else if (isHovered) SDL_SetRenderDrawColor(renderer, 100, 100, 140, 255);
        else SDL_SetRenderDrawColor(renderer, 70, 70, 90, 255);
        SDL_Rect dst{ Bounds.X, Bounds.Y, Bounds.Width, Bounds.Height };
        SDL_RenderFillRect(renderer, &dst);
    }

    // Draw text if present (slightly darken when pressed)
    if (!text.empty() && !fontKey.empty()) {
        uint8_t tr = fontR, tg = fontG, tb = fontB;
        if (isPressed) { tr = (uint8_t)std::max<int>(0, tr - 60); tg = (uint8_t)std::max<int>(0, tg - 60); tb = (uint8_t)std::max<int>(0, tb - 60); }
        auto ttex = WO3K::FontManager::RenderText(fontKey, text, tr, tg, tb);
        if (ttex && ttex->GetSDLTexture()) {
            int w = ttex->width();
            int h = ttex->height();
            SDL_Rect dst{ Bounds.X + (Bounds.Width - w) / 2, Bounds.Y + (Bounds.Height - h) / 2, w, h };
            SDL_RenderCopy(renderer, ttex->GetSDLTexture(), nullptr, &dst);
        }
    }

    // Focus outline
    auto focused = WO3K::FocusManager::GetFocus();
    bool drawFocus = isFocused || (focused && focused.get() == this);
    if (drawFocus) {
        // pulsing outline alpha using hoverProgress and a separate pulse based on time
        // compute pulse factor from hoverProgress and pressProgress
        float pulse = 0.6f + 0.4f * std::sin(hoverProgress * 3.1415f * 2.0f + pressProgress * 1.5f);
        uint8_t alpha = static_cast<uint8_t>(200 * std::clamp(pulse, 0.2f, 1.0f));
        SDL_SetRenderDrawColor(renderer, 255, 200, 60, alpha);
        SDL_Rect outline{ Bounds.X - 3, Bounds.Y - 3, Bounds.Width + 6, Bounds.Height + 6 };
        SDL_RenderDrawRect(renderer, &outline);
        outline = { Bounds.X - 1, Bounds.Y - 1, Bounds.Width + 2, Bounds.Height + 2 };
        SDL_RenderDrawRect(renderer, &outline);
    }
}

void Button::OnFocus() {
    isFocused = true;
}

void Button::OnBlur() {
    isFocused = false;
}

bool Button::HandleEvent(const void* event) {
    if (!event || !Visible || !Enabled) return false;

    const SDL_Event* ev = static_cast<const SDL_Event*>(event);
    if (!ev) return false;

    switch (ev->type) {
    case SDL_MOUSEBUTTONDOWN:
        if (ev->button.button == SDL_BUTTON_LEFT) {
            int x = ev->button.x;
            int y = ev->button.y;
            if (ContainsPoint(x, y)) {
                isPressed = true;
                // claim focus on press down as well
                try { FocusManager::SetFocus(shared_from_this()); } catch(...) {}
                // play press sound immediately
                if (!clickSoundKey.empty()) WO3K::SoundManager::Play(clickSoundKey);
                return true;
            }
        }
        break;
    case SDL_MOUSEBUTTONUP:
        if (ev->button.button == SDL_BUTTON_LEFT) {
            int x = ev->button.x;
            int y = ev->button.y;
            if (isPressed && ContainsPoint(x, y)) {
                isPressed = false;
                // click action
                if (!clickSoundKey.empty()) WO3K::SoundManager::Play(clickSoundKey, clickVolume);
                if (onClick) onClick(this);
                // set UI focus to this control when clicked
                try {
                    FocusManager::SetFocus(shared_from_this());
                } catch (...) {
                    // ignore if control is not managed by shared_ptr
                }
                return true;
            }
            isPressed = false;
        }
        break;
    case SDL_MOUSEMOTION:
        {
            int x = ev->motion.x;
            int y = ev->motion.y;
            bool nowHovered = ContainsPoint(x, y);
            if (nowHovered != isHovered) {
                isHovered = nowHovered;
                // play hover sound when entering hover
                if (isHovered && !hoverSoundKey.empty()) WO3K::SoundManager::Play(hoverSoundKey, hoverVolume);
            }
        }
        break;
    case SDL_KEYDOWN:
        if (ev->key.keysym.sym == SDLK_RETURN || ev->key.keysym.sym == SDLK_SPACE) {
            // keyboard activate if focused
            try {
                auto focused = FocusManager::GetFocus();
                if (focused && focused.get() == this) {
                    isPressed = true;
                    if (!clickSoundKey.empty()) WO3K::SoundManager::Play(clickSoundKey);
                    if (onClick) onClick(this);
                    return true;
                }
            } catch(...) {}
        }
        break;
    case SDL_KEYUP:
        if (ev->key.keysym.sym == SDLK_RETURN || ev->key.keysym.sym == SDLK_SPACE) {
            isPressed = false;
            return true;
        }
    default:
        break;
    }

    return false;
}

} // namespace WO3K
