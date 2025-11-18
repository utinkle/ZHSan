#pragma once

#include <memory>
#include <unordered_map>
#include "platform/Texture.h"

struct ShadowTiles {
    std::shared_ptr<Texture> corner; // r x r
    std::shared_ptr<Texture> edgeH;  // 1 x r (stretch horizontally)
    std::shared_ptr<Texture> edgeV;  // r x 1 (stretch vertically)
    std::shared_ptr<Texture> center; // 1 x 1 (stretch both)
    int radius = 0;
};

namespace WO3K {

class ShadowCache {
public:
    // Generate or retrieve cached shadow tiles for given radius using provided SDL_Renderer
    static ShadowTiles Get(int radius, SDL_Renderer* renderer);

private:
    static std::unordered_map<int, ShadowTiles> s_cache;
};

} // namespace WO3K
