#include "ui/ShadowCache.h"
#include "platform/PlatformSDL.h"
#include <SDL.h>
#include <algorithm>
#include <cmath>
#include <iostream>

using namespace WO3K;

std::unordered_map<int, ShadowTiles> ShadowCache::s_cache;

static std::shared_ptr<Texture> CreateTextureFromSurface(SDL_Renderer* renderer, SDL_Surface* surf)
{
    if (!renderer || !surf) return nullptr;
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
    if (!tex) return nullptr;
    auto ptr = std::make_shared<Texture>(tex, surf->w, surf->h);
    return ptr;
}

ShadowTiles ShadowCache::Get(int radius, SDL_Renderer* renderer)
{
    if (radius <= 0 || !renderer) return ShadowTiles();
    auto it = s_cache.find(radius);
    if (it != s_cache.end()) return it->second;

    ShadowTiles tiles;
    tiles.radius = radius;

    // sigma controls blur softness; choose sigma = radius/2
    double sigma = std::max(1.0, radius / 2.0);

    // Create corner surface (r x r)
    int r = radius;
    SDL_Surface* cornerSurf = SDL_CreateRGBSurfaceWithFormat(0, r, r, 32, SDL_PIXELFORMAT_RGBA32);
    if (!cornerSurf) { std::cerr << "ShadowCache: failed to create corner surface" << std::endl; return tiles; }
    SDL_LockSurface(cornerSurf);
    uint8_t* pixels = static_cast<uint8_t*>(cornerSurf->pixels);
    int pitch = cornerSurf->pitch;
    for (int y = 0; y < r; ++y) {
        for (int x = 0; x < r; ++x) {
            // distance from inner corner point at (r-1, r-1)
            double dx = (r - 1 - x);
            double dy = (r - 1 - y);
            double d2 = dx*dx + dy*dy;
            double a = std::exp(-d2 / (2.0 * sigma * sigma));
            uint8_t alpha = static_cast<uint8_t>(std::clamp(a * 255.0, 0.0, 255.0));
            int offset = y * pitch + x * 4;
            pixels[offset + 0] = 0; // B
            pixels[offset + 1] = 0; // G
            pixels[offset + 2] = 0; // R
            pixels[offset + 3] = alpha; // A
        }
    }
    SDL_UnlockSurface(cornerSurf);
    tiles.corner = CreateTextureFromSurface(renderer, cornerSurf);
    SDL_FreeSurface(cornerSurf);

    // Create edge vertical (r x 1) where inner edge at right (for left side)
    SDL_Surface* edgeV = SDL_CreateRGBSurfaceWithFormat(0, r, 1, 32, SDL_PIXELFORMAT_RGBA32);
    if (edgeV) {
        SDL_LockSurface(edgeV);
        uint8_t* p = static_cast<uint8_t*>(edgeV->pixels);
        int pp = edgeV->pitch;
        for (int x = 0; x < r; ++x) {
            double dx = (r - 1 - x);
            double a = std::exp(-(dx*dx) / (2.0 * sigma * sigma));
            uint8_t alpha = static_cast<uint8_t>(std::clamp(a * 255.0, 0.0, 255.0));
            int off = 0 * pp + x * 4;
            p[off + 0] = 0; p[off + 1] = 0; p[off + 2] = 0; p[off + 3] = alpha;
        }
        SDL_UnlockSurface(edgeV);
        tiles.edgeV = CreateTextureFromSurface(renderer, edgeV);
        SDL_FreeSurface(edgeV);
    }

    // Create edge horizontal (1 x r) where inner edge at bottom (for top side)
    SDL_Surface* edgeH = SDL_CreateRGBSurfaceWithFormat(0, 1, r, 32, SDL_PIXELFORMAT_RGBA32);
    if (edgeH) {
        SDL_LockSurface(edgeH);
        uint8_t* p = static_cast<uint8_t*>(edgeH->pixels);
        int pp = edgeH->pitch;
        for (int y = 0; y < r; ++y) {
            double dy = (r - 1 - y);
            double a = std::exp(-(dy*dy) / (2.0 * sigma * sigma));
            uint8_t alpha = static_cast<uint8_t>(std::clamp(a * 255.0, 0.0, 255.0));
            int off = y * pp + 0 * 4;
            p[off + 0] = 0; p[off + 1] = 0; p[off + 2] = 0; p[off + 3] = alpha;
        }
        SDL_UnlockSurface(edgeH);
        tiles.edgeH = CreateTextureFromSurface(renderer, edgeH);
        SDL_FreeSurface(edgeH);
    }

    // center 1x1 opaque-ish pixel
    SDL_Surface* center = SDL_CreateRGBSurfaceWithFormat(0, 1, 1, 32, SDL_PIXELFORMAT_RGBA32);
    if (center) {
        SDL_LockSurface(center);
        uint8_t* p = static_cast<uint8_t*>(center->pixels);
        p[0] = 0; p[1] = 0; p[2] = 0; p[3] = 200;
        SDL_UnlockSurface(center);
        tiles.center = CreateTextureFromSurface(renderer, center);
        SDL_FreeSurface(center);
    }

    s_cache[radius] = tiles;
    return tiles;
}
