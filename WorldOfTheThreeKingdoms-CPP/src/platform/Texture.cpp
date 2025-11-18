#include "Texture.h"
#include <SDL.h>
#ifdef SDL_IMAGE_FOUND
#include <SDL_image.h>
#endif
#include <iostream>

Texture::Texture(SDL_Texture* tex, int w, int h)
    : m_texture(tex), m_width(w), m_height(h)
{
}

Texture::~Texture()
{
    if (m_texture)
        SDL_DestroyTexture(m_texture);
}

std::shared_ptr<Texture> Texture::LoadFromFile(SDL_Renderer* renderer, const std::string& path)
{
    if (!renderer)
        return nullptr;

    SDL_Surface* surface = nullptr;
#ifdef SDL_IMAGE_FOUND
    surface = IMG_Load(path.c_str());
    if (!surface)
    {
        std::cerr << "IMG_Load failed for " << path << " : " << IMG_GetError() << std::endl;
    }
#endif
    if (!surface)
    {
        // fallback to SDL_LoadBMP
        surface = SDL_LoadBMP(path.c_str());
        if (!surface)
        {
            std::cerr << "SDL_LoadBMP failed for " << path << " : " << SDL_GetError() << std::endl;
            return nullptr;
        }
    }

    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surface);
    if (!tex)
    {
        std::cerr << "SDL_CreateTextureFromSurface failed: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surface);
        return nullptr;
    }

    int w = surface->w;
    int h = surface->h;
    SDL_FreeSurface(surface);

    return std::make_shared<Texture>(tex, w, h);
}
