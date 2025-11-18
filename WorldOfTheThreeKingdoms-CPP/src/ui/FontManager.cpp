#include "ui/FontManager.h"
#include "platform/PlatformSDL.h"
#include <iostream>

#ifdef SDL_TTF_FOUND
#include <SDL_ttf.h>
#endif

namespace WO3K {

static std::unordered_map<std::string, std::shared_ptr<Texture>> g_textCache;

bool FontManager::Init()
{
#ifdef SDL_TTF_FOUND
    if (TTF_WasInit() == 0) {
        if (TTF_Init() == -1) {
            std::cerr << "FontManager: TTF_Init failed: " << TTF_GetError() << std::endl;
            return false;
        }
    }
    return true;
#else
    // SDL_ttf not available — text rendering is disabled
    std::cerr << "FontManager: SDL_ttf not available; text rendering disabled." << std::endl;
    return false;
#endif
}

std::shared_ptr<Texture> FontManager::RenderText(const std::string& fontKey, const std::string& text, uint8_t r, uint8_t g, uint8_t b)
{
    // cache key
    std::string cacheKey = fontKey + "::" + text + ":" + std::to_string(r) + "," + std::to_string(g) + "," + std::to_string(b);
    auto it = g_textCache.find(cacheKey);
    if (it != g_textCache.end()) return it->second;

#ifdef SDL_TTF_FOUND
    // fontKey format: path:ptsize or just path (default 16)
    std::string path = fontKey;
    int psize = 16;
    auto pos = fontKey.find(':');
    if (pos != std::string::npos) {
        path = fontKey.substr(0, pos);
        psize = std::stoi(fontKey.substr(pos + 1));
    }

    TTF_Font* font = TTF_OpenFont(path.c_str(), psize);
    if (!font) {
        std::cerr << "FontManager: TTF_OpenFont failed for " << path << ": " << TTF_GetError() << std::endl;
        return nullptr;
    }

    SDL_Color color{ r, g, b, 255 };
    SDL_Surface* surf = TTF_RenderUTF8_Blended(font, text.c_str(), color);
    if (!surf) {
        std::cerr << "FontManager: TTF_RenderUTF8_Blended failed: " << TTF_GetError() << std::endl;
        TTF_CloseFont(font);
        return nullptr;
    }

    SDL_Renderer* renderer = Platform::GetSDLRenderer();
    if (!renderer) {
        SDL_FreeSurface(surf);
        TTF_CloseFont(font);
        return nullptr;
    }

    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
    if (!tex) {
        std::cerr << "FontManager: SDL_CreateTextureFromSurface failed: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surf);
        TTF_CloseFont(font);
        return nullptr;
    }

    int w = surf->w;
    int h = surf->h;
    SDL_FreeSurface(surf);
    TTF_CloseFont(font);

    auto texture = std::make_shared<Texture>(tex, w, h);
    g_textCache[cacheKey] = texture;
    return texture;
#else
    (void)fontKey; (void)text; (void)r; (void)g; (void)b;
    return nullptr;
#endif
}

void FontManager::ClearCache()
{
    g_textCache.clear();
}

} // namespace WO3K
