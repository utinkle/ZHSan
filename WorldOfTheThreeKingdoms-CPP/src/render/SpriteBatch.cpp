#include "SpriteBatch.h"
#include "render/Camera.h"
#include <SDL.h>

SpriteBatch::SpriteBatch(SDL_Renderer* renderer)
    : m_renderer(renderer)
{
}

SpriteBatch::~SpriteBatch()
{
}

void SpriteBatch::Begin()
{
    // SDL renderer doesn't require explicit begin for now
}

void SpriteBatch::End()
{
    // nothing for now
}

void SpriteBatch::Draw(SDL_Texture* texture, const SDL_Rect* src, const SDL_Rect* dest, double rotation, const SDL_Point* center, SDL_RendererFlip flip)
{
    if (!texture || !m_renderer)
        return;

    // Apply camera offset if any: offset dest rect by camera X/Y
    SDL_Rect localDest;
    const SDL_Rect* useDest = dest;
    if (dest) {
        int cx = 0, cy = 0, cw = 0, ch = 0;
        WO3K::Camera::Instance().GetView(cx, cy, cw, ch);
        localDest = *dest;
        localDest.x -= cx;
        localDest.y -= cy;
        useDest = &localDest;
    }

    SDL_RenderCopyEx(m_renderer, texture, src, useDest, rotation, const_cast<SDL_Point*>(center), flip);
}
