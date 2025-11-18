#include "screen/MainMenuScreen.h"
#include "ui/Button.h"
#include "game/MainGame.h"
#include "game/CacheManager.h"
#include "ui/FontManager.h"
#include "screen/LoadingScreen.h"
#include <iostream>

using namespace WO3K;

void MainMenuScreen::Initialize()
{
    // Create root panel
    rootPanel = std::make_shared<Panel>();

    // Compute sizes based on current window
    int ww = 800, wh = 600;
    if (GameManager::MainGame::Current) {
        ww = GameManager::MainGame::Current->GetWindowWidth();
        wh = GameManager::MainGame::Current->GetWindowHeight();
    }

    int btnW = 300;
    int btnH = 48;
    int spacing = 12;
    int totalH = btnH * 3 + spacing * 2;
    int startY = (wh - totalH) / 2;
    int centerX = (ww - btnW) / 2;

    // Start Game button
    auto btnStart = std::make_shared<Button>();
    btnStart->Bounds = { centerX, startY, btnW, btnH };
    btnStart->SetText(u8"\u5F00\u59CB\u6E38\u620F");
    btnStart->SetBackgroundKey("UI/button");
    btnStart->SetHoverTextureKey("UI/button_hover");
    btnStart->SetPressedTextureKey("UI/button_pressed");
    btnStart->SetHoverSoundKey("button_hover");
    btnStart->SetClickSoundKey("button_click");
    btnStart->SetFontKey("Content/Font/SimHei.ttf:16");
    btnStart->SetOnClick([](Button* b) {
        std::cout << "MainMenu: Start clicked - pushing LoadingScreen" << std::endl;
        try {
            auto loading = std::make_shared<WO3K::LoadingScreen>();
            loading->Initialize();
            if (GameManager::MainGame::Current) GameManager::MainGame::Current->PushScreen(loading);
        } catch (...) {
            std::cerr << "MainMenu: Failed to push LoadingScreen" << std::endl;
        }
    });

    // Options button
    auto btnOptions = std::make_shared<Button>();
    btnOptions->Bounds = { centerX, startY + (btnH + spacing) * 1, btnW, btnH };
    btnOptions->SetText(u8"\u9009\u9879");
    btnOptions->SetBackgroundKey("UI/button");
    btnOptions->SetHoverTextureKey("UI/button_hover");
    btnOptions->SetPressedTextureKey("UI/button_pressed");
    btnOptions->SetHoverSoundKey("button_hover");
    btnOptions->SetClickSoundKey("button_click");
    btnOptions->SetFontKey("Content/Font/SimHei.ttf:16");
    btnOptions->SetOnClick([](Button* b) {
        std::cout << "MainMenu: Options clicked" << std::endl;
    });

    // Exit button
    auto btnExit = std::make_shared<Button>();
    btnExit->Bounds = { centerX, startY + (btnH + spacing) * 2, btnW, btnH };
    btnExit->SetText(u8"\u9000\u51FA");
    btnExit->SetBackgroundKey("UI/button");
    btnExit->SetHoverTextureKey("UI/button_hover");
    btnExit->SetPressedTextureKey("UI/button_pressed");
    btnExit->SetHoverSoundKey("button_hover");
    btnExit->SetClickSoundKey("button_click");
    btnExit->SetFontKey("Content/Font/SimHei.ttf:16");
    btnExit->SetOnClick([](Button* b) {
        std::cout << "MainMenu: Exit clicked - requesting exit" << std::endl;
        if (GameManager::MainGame::Current) GameManager::MainGame::Current->Exit();
    });

    rootPanel->AddChild(btnStart);
    rootPanel->AddChild(btnOptions);
    rootPanel->AddChild(btnExit);

    // Initialize children (register focusable controls)
    rootPanel->Initialize();
}

void MainMenuScreen::OnEnter()
{
    // Ensure fonts are initialized (safe to call repeatedly)
    WO3K::FontManager::Init();
}

void MainMenuScreen::Update(double deltaTime)
{
    if (rootPanel) rootPanel->Update(deltaTime);
}

void MainMenuScreen::Draw()
{
    if (rootPanel) rootPanel->Draw();
}

bool MainMenuScreen::HandleEvent(const void* event)
{
    if (rootPanel) return rootPanel->HandleEvent(event);
    return false;
}
