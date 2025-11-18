#include "screen/GameScreen.h"
#include "ui/FontManager.h"
#include "platform/PlatformSDL.h"
#include "game/MainGame.h"
#include <SDL.h>

using namespace WO3K;

void GameScreen::Draw()
{
    SDL_Renderer* renderer = Platform::GetSDLRenderer();
    if (!renderer) return;

    // Clear to a different color
    SDL_SetRenderDrawColor(renderer, 40, 40, 60, 255);
    SDL_RenderClear(renderer);

    // Draw "Game Started" text
    auto tex = FontManager::RenderText("Content/Font/SimHei.ttf:20", "\u6E38\u620F\u5DF2\u5F00\u59CB", 230, 230, 230);
    if (tex && tex->GetSDLTexture()) {
        int w = tex->width();
        int h = tex->height();
        int cx =  (GameManager::MainGame::Current ? GameManager::MainGame::Current->GetWindowWidth() : 800);
        int cy =  (GameManager::MainGame::Current ? GameManager::MainGame::Current->GetWindowHeight() : 600);
        SDL_Rect dst{ (cx - w) / 2, (cy - h) / 2, w, h };
        SDL_RenderCopy(renderer, tex->GetSDLTexture(), nullptr, &dst);
    }
}
