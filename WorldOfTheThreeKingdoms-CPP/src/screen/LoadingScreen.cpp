#include "screen/LoadingScreen.h"
#include "game/MainGame.h"
#include "screen/GameScreen.h"
#include "ui/FontManager.h"
#include "platform/PlatformSDL.h"
#include <SDL.h>
#include <iostream>

using namespace WO3K;

void LoadingScreen::Initialize()
{
    m_start = std::chrono::high_resolution_clock::now();
    m_elapsed = 0.0;
    // Ensure font system is ready
    FontManager::Init();
}

void LoadingScreen::Update(double deltaTime)
{
    m_elapsed += deltaTime;
    // Simulate loading for 1.2 seconds then switch to GameScreen
    if (m_elapsed >= 1.2) {
        std::cout << "LoadingScreen: Loading complete, switching to GameScreen" << std::endl;
        // Pop this screen and push game screen
        if (GameManager::MainGame::Current) {
            auto screenMgr = GameManager::MainGame::Current->GetScreenManager();
            if (screenMgr) {
                // Replace loading with game screen
                auto game = std::make_shared<GameScreen>();
                screenMgr->Replace(game);
            }
        }
    }
}

void LoadingScreen::Draw()
{
    SDL_Renderer* renderer = Platform::GetSDLRenderer();
    if (!renderer) return;

    // Clear with dark background (overlay)
    SDL_SetRenderDrawColor(renderer, 20, 20, 30, 255);
    SDL_RenderClear(renderer);

    // Draw loading text centered
    auto tex = FontManager::RenderText("Content/Font/SimHei.ttf:20", "\u52A0\u8F7D\u4E2D...", 255, 255, 255);
    if (tex && tex->GetSDLTexture()) {
        int w = tex->width();
        int h = tex->height();
        int cx =  (GameManager::MainGame::Current ? GameManager::MainGame::Current->GetWindowWidth() : 800);
        int cy =  (GameManager::MainGame::Current ? GameManager::MainGame::Current->GetWindowHeight() : 600);
        SDL_Rect dst{ (cx - w) / 2, (cy - h) / 2, w, h };
        SDL_RenderCopy(renderer, tex->GetSDLTexture(), nullptr, &dst);
    }
}
