
#include "PlatformSDL.h"
#include <SDL.h>
#ifdef SDL_IMAGE_FOUND
#include <SDL_image.h>
#endif
#ifdef SDL_MIXER_FOUND
#include <SDL_mixer.h>
#endif
#include <iostream>

namespace Platform {

static SDL_Window* g_window = nullptr;
static SDL_Renderer* g_renderer = nullptr;

bool Init(const std::string &title, int width, int height)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
    {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return false;
    }

#ifdef SDL_IMAGE_FOUND
    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    int initted = IMG_Init(imgFlags);
    if ((initted & imgFlags) != imgFlags)
    {
        std::cerr << "IMG_Init failed: " << IMG_GetError() << std::endl;
        // continue; PNG/JPG support might be partial
    }
#endif

#ifdef SDL_MIXER_FOUND
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        std::cerr << "Mix_OpenAudio failed: " << Mix_GetError() << std::endl;
        // continue without audio
    }
#endif

    g_window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    if (!g_window)
    {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        return false;
    }

    g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!g_renderer)
    {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(g_window);
        g_window = nullptr;
        return false;
    }

    return true;
}

void Shutdown()
{
#ifdef SDL_MIXER_FOUND
    Mix_CloseAudio();
    Mix_Quit();
#endif
#ifdef SDL_IMAGE_FOUND
    IMG_Quit();
#endif

    if (g_renderer)
    {
        SDL_DestroyRenderer(g_renderer);
        g_renderer = nullptr;
    }
    if (g_window)
    {
        SDL_DestroyWindow(g_window);
        g_window = nullptr;
    }

    SDL_Quit();
}

SDL_Window* GetSDLWindow() { return g_window; }
SDL_Renderer* GetSDLRenderer() { return g_renderer; }

} // namespace Platform
