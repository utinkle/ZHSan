#include "screen/MainGameScreen.h"
#include "ui/Button.h"
#include "ui/ListView.h"
#include "ui/FlowPanel.h"
#include "ui/BoxPanel.h"
#include "ui/FontManager.h"
#include "game/CacheManager.h"
#include "platform/Texture.h"
#include "render/Camera.h"
#include "game/MainGame.h"
#include "platform/PlatformSDL.h"
#include <SDL.h>
#include <iostream>

using namespace WO3K;

void MainGameScreen::Initialize()
{
    rootPanel = std::make_shared<Panel>();

    // Set root panel to cover the whole window
    int ww =  GameManager::MainGame::Current ? GameManager::MainGame::Current->GetWindowWidth() : 1024;
    int wh =  GameManager::MainGame::Current ? GameManager::MainGame::Current->GetWindowHeight() : 768;
    rootPanel->Bounds = { 0, 0, ww, wh };

    // Create a top toolbar as a FlowPanel with horizontal flow
    int btnW = 100, btnH = 36, margin = 8;
    auto topBar = std::make_shared<FlowPanel>();
    topBar->DockMode = Control::Dock::Top;
    topBar->Bounds.Height = btnH + margin*2;
    topBar->Padding = margin;
    topBar->FlowDirection = FlowPanel::Direction::Horizontal;
    rootPanel->AddChild(topBar);

    auto b1 = std::make_shared<Button>();
    b1->Bounds.Width = btnW; b1->Bounds.Height = btnH;
    b1->SetText("菜单");
    b1->SetBackgroundKey("UI/button");
    b1->SetHoverTextureKey("UI/button_hover");
    b1->SetPressedTextureKey("UI/button_pressed");
    b1->SetFontKey("Content/Font/SimHei.ttf:14");
    topBar->AddChild(b1);

    auto b2 = std::make_shared<Button>();
    b2->Bounds.Width = btnW; b2->Bounds.Height = btnH;
    b2->SetText("存档");
    b2->SetBackgroundKey("UI/button");
    b2->SetHoverTextureKey("UI/button_hover");
    b2->SetPressedTextureKey("UI/button_pressed");
    b2->SetFontKey("Content/Font/SimHei.ttf:14");
    topBar->AddChild(b2);

    // Right-side unit panel: use ListView docked to Right
    int uw = 160;
    unitList = std::make_shared<ListView>();
    unitList->DockMode = Control::Dock::Right;
    unitList->Bounds.Width = uw; // preferred width
    unitList->Margin.Top = margin; unitList->Margin.Right = margin; unitList->Margin.Bottom = margin;
    unitList->OnItemActivated = [](int idx) {
        // placeholder: when item activated, log
        SDL_Log("Unit %d activated", idx+1);
    };
    rootPanel->AddChild(unitList);

    // Left mini-map / info panel
    leftBoxPanel = std::make_shared<BoxPanel>();
    leftBoxPanel->DockMode = Control::Dock::Left;
    leftBoxPanel->Bounds.Width = 240;
    leftBoxPanel->Margin.Left = margin; leftBoxPanel->Margin.Top = margin; leftBoxPanel->Margin.Bottom = margin;
    leftBoxPanel->DrawBorder = true;
    // slightly lighter background for HUD region
    leftBoxPanel->R = 48; leftBoxPanel->G = 54; leftBoxPanel->B = 64; leftBoxPanel->A = 255;
    leftBoxPanel->Title = "迷你地图";
    leftBoxPanel->TitleHeight = 24;
    leftBoxPanel->RoundedRadius = 8;
    leftBoxPanel->DrawShadow = true;
    leftBoxPanel->ShadowOffsetX = 4; leftBoxPanel->ShadowOffsetY = 4;
    leftBoxPanel->HandleVisible = true;
    rootPanel->AddChild(leftBoxPanel);

    // bottom toolbar as a FlowPanel docked Bottom
    int tbh = 44; int tbw = 80;
    auto bottomBar = std::make_shared<FlowPanel>();
    bottomBar->DockMode = Control::Dock::Bottom;
    bottomBar->Bounds.Height = tbh + margin*2;
    bottomBar->Padding = margin;
    bottomBar->FlowDirection = FlowPanel::Direction::Horizontal;
    rootPanel->AddChild(bottomBar);

    for (int j = 0; j < 5; ++j) {
        auto act = std::make_shared<Button>();
        act->Bounds.Width = tbw; act->Bounds.Height = tbh;
        act->SetText(("行动" + std::to_string(j+1)).c_str());
        act->SetBackgroundKey("UI/button");
        act->SetHoverTextureKey("UI/button_hover");
        act->SetPressedTextureKey("UI/button_pressed");
        act->SetFontKey("Content/Font/SimHei.ttf:14");
        bottomBar->AddChild(act);
    }

    // try load a mini-map texture (key is relative to Content/Textures/) - keep existing overview first
    miniMapTexture = GameManager::CacheManager::LoadTexture("Maps/overview", GameManager::CacheType::Live);
    // If not present, try the raw ContentLite path provided by user (project asset)
    if (!miniMapTexture) {
        const std::string neoPath = "ContentLite/Textures/Resources/ditu/_neo7.jpg";
        miniMapTexture = GameManager::CacheManager::LoadTexture(neoPath, GameManager::CacheType::Live);
        if (miniMapTexture) {
            // register an alias so other code can refer to the stable key "Maps/overview"
            GameManager::CacheManager::RegisterTextureAlias("Maps/overview", neoPath);
            // also register a simple record for discovery/debugging
            GameManager::CacheManager::RegisterTextureRecord("Maps/overview", { "Maps/overview", GameManager::CacheType::Live, ".jpg" });
        }
    }

    // if we have a mini-map texture, use its pixel size as the world dimensions
    if (miniMapTexture) {
        worldWidth = miniMapTexture->width();
        worldHeight = miniMapTexture->height();
    }
    // default camera size (clamped to world)
    camW = std::min(worldWidth, 512);
    camH = std::min(worldHeight, 384);
    camX = (worldWidth - camW) / 2;
    camY = (worldHeight - camH) / 2;
    // apply to global camera
    WO3K::Camera::Instance().SetView(camX, camY, camW, camH);

    // HUD sample text will be drawn by FontManager in Draw()
    rootPanel->Initialize();

    // populate HUD/unit list from provider (if any)
    RefreshHUD();
    // If no provider set, fall back to MainGame's default unit list provider
    if (!unitProvider && GameManager::MainGame::Current) {
        unitProvider = []() -> std::vector<std::string> {
            if (GameManager::MainGame::Current) return GameManager::MainGame::Current->GetUnitNames();
            return {};
        };
        RefreshHUD();
    }
}

void MainGameScreen::RefreshHUD()
{
    if (!unitList) return;
    unitList->Items.clear();
    if (unitProvider) {
        try {
            auto names = unitProvider();
            for (auto &n : names) unitList->Items.push_back(n);
        } catch(...) {
            // swallow provider exceptions and leave list empty
        }
    }

    // fallback: sample content if provider produced nothing
    if (unitList->Items.empty()) {
        for (int i = 0; i < 12; ++i) {
            unitList->Items.push_back(std::string("武将") + std::to_string(i+1));
        }
    }
}

void MainGameScreen::OnResize(int width, int height) {
    if (!rootPanel) return;
    rootPanel->Bounds = { 0, 0, width, height };
    rootPanel->ApplyDockLayout();
}

void MainGameScreen::Update(double deltaTime)
{
    if (rootPanel) rootPanel->Update(deltaTime);

    // smooth camera interpolation towards target when not directly dragging the viewport
    if (!draggingViewport) {
        float t = std::min(1.0f, camLerpSpeed * static_cast<float>(deltaTime));
        // lerp camX/camY towards camTargetX/camTargetY
        camX = static_cast<int>(camX + (camTargetX - camX) * t);
        camY = static_cast<int>(camY + (camTargetY - camY) * t);
        // apply to global camera
        WO3K::Camera::Instance().SetView(camX, camY, camW, camH);
    }
}

void MainGameScreen::Draw()
{
    if (rootPanel) rootPanel->Draw();

    // draw some HUD text
    SDL_Renderer* renderer = Platform::GetSDLRenderer();
    if (!renderer) return;
    auto t = FontManager::RenderText("Content/Font/SimHei.ttf:16", "生命: 100/100", 220, 220, 220);
    if (t && t->GetSDLTexture()) {
        SDL_Rect dst{ 20, 80, t->width(), t->height() };
        SDL_RenderCopy(renderer, t->GetSDLTexture(), nullptr, &dst);
    }

    // Draw mini-map inside leftBoxPanel
    if (leftBoxPanel) {
        SDL_Rect lb{ leftBoxPanel->Bounds.X + 8, leftBoxPanel->Bounds.Y + 8,
                     std::max(16, leftBoxPanel->Bounds.Width - 16), std::max(16, leftBoxPanel->Bounds.Height - 16) };

        // draw mini-map texture if available, otherwise fallback to solid background
        if (miniMapTexture && miniMapTexture->GetSDLTexture()) {
            SDL_RenderCopy(renderer, miniMapTexture->GetSDLTexture(), nullptr, &lb);
        } else {
            SDL_SetRenderDrawColor(renderer, 20, 28, 36, 255);
            SDL_RenderFillRect(renderer, &lb);
            // sample markers when no texture
            SDL_SetRenderDrawColor(renderer, 200, 60, 60, 255);
            int ox = lb.x + 6, oy = lb.y + 6;
            SDL_Rect p1{ ox, oy, 4, 4 };
            SDL_Rect p2{ ox + lb.w/3, oy + lb.h/4, 4, 4 };
            SDL_Rect p3{ ox + lb.w*2/3, oy + lb.h*2/3, 4, 4 };
            SDL_RenderFillRect(renderer, &p1);
            SDL_RenderFillRect(renderer, &p2);
            SDL_RenderFillRect(renderer, &p3);
        }

        // compute viewport rect mapping camera -> mini-map coords
        float fx = (worldWidth > 0) ? (static_cast<float>(camX) / static_cast<float>(worldWidth)) : 0.0f;
        float fy = (worldHeight > 0) ? (static_cast<float>(camY) / static_cast<float>(worldHeight)) : 0.0f;
        float fw = (worldWidth > 0) ? (static_cast<float>(camW) / static_cast<float>(worldWidth)) : 1.0f;
        float fh = (worldHeight > 0) ? (static_cast<float>(camH) / static_cast<float>(worldHeight)) : 1.0f;

        SDL_Rect vrect{
            lb.x + static_cast<int>(fx * lb.w),
            lb.y + static_cast<int>(fy * lb.h),
            std::max(2, static_cast<int>(fw * lb.w)),
            std::max(2, static_cast<int>(fh * lb.h))
        };

        // translucent fill
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 50, 150, 220, 80);
        SDL_RenderFillRect(renderer, &vrect);
        // border
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        SDL_SetRenderDrawColor(renderer, 80, 200, 255, 200);
        SDL_RenderDrawRect(renderer, &vrect);

        // label "迷你地图"
        auto lt = FontManager::RenderText("Content/Font/SimHei.ttf:12", "迷你地图", 200, 200, 200);
        if (lt && lt->GetSDLTexture()) {
            SDL_Rect ldst{ leftBoxPanel->Bounds.X + 10, leftBoxPanel->Bounds.Y + 10 + lb.h, lt->width(), lt->height() };
            if (ldst.x + ldst.w < leftBoxPanel->Bounds.X + leftBoxPanel->Bounds.Width - 8)
                SDL_RenderCopy(renderer, lt->GetSDLTexture(), nullptr, &ldst);
        }
    }
}

bool MainGameScreen::HandleEvent(const void* event)
{
    const SDL_Event* ev = static_cast<const SDL_Event*>(event);
    // first let UI controls handle it
    if (rootPanel && rootPanel->HandleEvent(event)) return true;

    if (!leftBoxPanel) return false;

    // compute mini-map dest rect
    SDL_Rect lb{ leftBoxPanel->Bounds.X + 8, leftBoxPanel->Bounds.Y + 8,
                 std::max(16, leftBoxPanel->Bounds.Width - 16), std::max(16, leftBoxPanel->Bounds.Height - 16) };

    if (!ev) return false;

    // compute viewport rect again (same math as in Draw)
    float fx = (worldWidth > 0) ? (static_cast<float>(camX) / static_cast<float>(worldWidth)) : 0.0f;
    float fy = (worldHeight > 0) ? (static_cast<float>(camY) / static_cast<float>(worldHeight)) : 0.0f;
    float fw = (worldWidth > 0) ? (static_cast<float>(camW) / static_cast<float>(worldWidth)) : 1.0f;
    float fh = (worldHeight > 0) ? (static_cast<float>(camH) / static_cast<float>(worldHeight)) : 1.0f;
    SDL_Rect vrect{
        lb.x + static_cast<int>(fx * lb.w),
        lb.y + static_cast<int>(fy * lb.h),
        std::max(2, static_cast<int>(fw * lb.w)),
        std::max(2, static_cast<int>(fh * lb.h))
    };

    if (ev->type == SDL_MOUSEBUTTONDOWN && ev->button.button == SDL_BUTTON_LEFT) {
        int mx = ev->button.x, my = ev->button.y;
        if (mx >= vrect.x && mx <= vrect.x + vrect.w && my >= vrect.y && my <= vrect.y + vrect.h) {
            // start dragging viewport
            draggingViewport = true;
            viewportDragOffsetX = mx - vrect.x;
            viewportDragOffsetY = my - vrect.y;
            return true;
        }
        if (mx >= lb.x && mx <= lb.x + lb.w && my >= lb.y && my <= lb.y + lb.h) {
            // click on mini-map to set camera target (smooth)
            draggingMiniMap = true;
            float nx = float(mx - lb.x) / float(lb.w);
            float ny = float(my - lb.y) / float(lb.h);
            camTargetX = int(nx * worldWidth) - camW / 2;
            camTargetY = int(ny * worldHeight) - camH / 2;
            // clamp target
            camTargetX = std::max(0, std::min(camTargetX, worldWidth - camW));
            camTargetY = std::max(0, std::min(camTargetY, worldHeight - camH));
            return true;
        }
    } else if (ev->type == SDL_MOUSEBUTTONUP && ev->button.button == SDL_BUTTON_LEFT) {
        draggingMiniMap = false;
        draggingViewport = false;
        return false;
    } else if (ev->type == SDL_MOUSEMOTION) {
        int mx = ev->motion.x, my = ev->motion.y;
        if (draggingViewport) {
            // move viewport so its top-left = mouse - offset
            int nx = mx - viewportDragOffsetX;
            int ny = my - viewportDragOffsetY;
            // map to world coordinates
            float px = float(nx - lb.x) / float(lb.w);
            float py = float(ny - lb.y) / float(lb.h);
            camX = int(px * worldWidth);
            camY = int(py * worldHeight);
            // adjust to make top-left mapping consistent (clamp)
            camX = std::max(0, std::min(camX, worldWidth - camW));
            camY = std::max(0, std::min(camY, worldHeight - camH));
            // when dragging viewport, set target to current for immediate response
            camTargetX = camX;
            camTargetY = camY;
            WO3K::Camera::Instance().SetView(camX, camY, camW, camH);
            return true;
        } else if (draggingMiniMap) {
            // adjust target while dragging mini-map background
            float nx = float(mx - lb.x) / float(lb.w);
            float ny = float(my - lb.y) / float(lb.h);
            camTargetX = int(nx * worldWidth) - camW / 2;
            camTargetY = int(ny * worldHeight) - camH / 2;
            camTargetX = std::max(0, std::min(camTargetX, worldWidth - camW));
            camTargetY = std::max(0, std::min(camTargetY, worldHeight - camH));
            return true;
        }
    }

    return false;
}
