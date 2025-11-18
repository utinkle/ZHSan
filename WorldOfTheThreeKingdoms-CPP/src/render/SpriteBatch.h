#pragma once

#include <SDL.h>
#include <memory>

class SpriteBatch {
public:
    explicit SpriteBatch(SDL_Renderer* renderer);
    ~SpriteBatch();

    void Begin();
    void End();

    // Draw texture with optional source/dest rects
    void Draw(SDL_Texture* texture, const SDL_Rect* src, const SDL_Rect* dest, double rotation = 0.0, const SDL_Point* center = nullptr, SDL_RendererFlip flip = SDL_FLIP_NONE);

private:
    SDL_Renderer* m_renderer;
};
