#pragma once

#include <SDL.h>
#include <memory>
#include <string>

class Texture {
public:
    static std::shared_ptr<Texture> LoadFromFile(SDL_Renderer* renderer, const std::string& path);
    ~Texture();

    SDL_Texture* GetSDLTexture() const { return m_texture; }
    int width() const { return m_width; }
    int height() const { return m_height; }

    // constructor is public so make_shared can access it
    Texture(SDL_Texture* tex, int w, int h);

private:
    SDL_Texture* m_texture;
    int m_width;
    int m_height;
};
