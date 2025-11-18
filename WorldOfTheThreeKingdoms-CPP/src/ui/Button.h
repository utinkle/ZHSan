#pragma once

#include "ui/Control.h"
#include <functional>
#include <string>

namespace WO3K {

class Button : public Control {
public:
    using ClickHandler = std::function<void(Button*)>;

    Button() {
        Focusable = true;
    }
    ~Button() override = default;
    Button(const Button&) = delete;


    void SetText(const std::string& t) { text = t; }
    const std::string& GetText() const { return text; }

    void SetBackgroundKey(const std::string& key) { bgTextureKey = key; }
    void SetHoverTextureKey(const std::string& key) { hoverTextureKey = key; }
    void SetPressedTextureKey(const std::string& key) { pressedTextureKey = key; }
    void SetHoverSoundKey(const std::string& key) { hoverSoundKey = key; }
    void SetClickSoundKey(const std::string& key) { clickSoundKey = key; }
    void SetHoverVolume(float v) { hoverVolume = v; }
    void SetClickVolume(float v) { clickVolume = v; }
    void SetFontKey(const std::string& key) { fontKey = key; }
    void SetFontColor(uint8_t r, uint8_t g, uint8_t b) { fontR = r; fontG = g; fontB = b; }

    void SetOnClick(ClickHandler h) { onClick = std::move(h); }

    void Update(double deltaTime) override;
    void Draw() override;
    bool HandleEvent(const void* event) override;
    void OnFocus() override;
    void OnBlur() override;

private:
    std::string text;
    ClickHandler onClick;
    bool isPressed = false;
    bool isHovered = false;
    bool isFocused = false;
    // animation progress (0.0..1.0)
    float hoverProgress = 0.0f;
    float pressProgress = 0.0f;
    // animation speeds
    float hoverSpeed = 6.0f; // per-second
    float pressSpeed = 12.0f;
    std::string bgTextureKey;
    std::string hoverTextureKey;
    std::string pressedTextureKey;
    std::string hoverSoundKey;
    std::string clickSoundKey;
    float hoverVolume = 1.0f;
    float clickVolume = 1.0f;
    std::string fontKey;
    uint8_t fontR = 255, fontG = 255, fontB = 255;
};

} // namespace WO3K
