#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include "platform/Texture.h"

namespace WO3K {

class FontManager {
public:
    // Initialize font subsystem; returns true if SDL_ttf is available or a fallback is ready
    static bool Init();

    // Render text to a Texture (cached). fontKey is a logical identifier like "Assets/Font/simsun.ttf:16"
    static std::shared_ptr<Texture> RenderText(const std::string& fontKey, const std::string& text, uint8_t r = 255, uint8_t g = 255, uint8_t b = 255);

    // Clear cached rendered text
    static void ClearCache();
};

} // namespace WO3K
