
#pragma once

#include <SDL.h>
#ifdef SDL_IMAGE_FOUND
#include <SDL_image.h>
#endif
#ifdef SDL_MIXER_FOUND
#include <SDL_mixer.h>
#endif
#include <string>

namespace Platform {

bool Init(const std::string &title = "WorldOfTheThreeKingdomsCPP", int width = 1024, int height = 768);
void Shutdown();
SDL_Window* GetSDLWindow();
SDL_Renderer* GetSDLRenderer();

} // namespace Platform
