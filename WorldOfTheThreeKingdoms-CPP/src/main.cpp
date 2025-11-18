


#include <iostream>
#include <memory>
#include <filesystem>
#include <optional>

#define SDL_MAIN_HANDLED
#include "platform/PlatformSDL.h"
#include "render/SpriteBatch.h"
#include "platform/Texture.h"
#include "game/MainGame.h"
#include "screen/MainMenuScreen.h"

namespace fs = std::filesystem;

/// <summary>
/// Simple demo game that extends MainGame to test the framework
/// </summary>
class DemoGame : public GameManager::MainGame {
protected:
    std::shared_ptr<Texture> m_testTexture;

    bool OnInitialize() override
    {
        std::cout << "DemoGame: Initializing..." << std::endl;
        // Push the main menu screen
        try {
            auto menu = std::make_shared<WO3K::MainMenuScreen>();
            menu->Initialize();
            if (GameManager::MainGame::Current) GameManager::MainGame::Current->PushScreen(menu);
        } catch (...) {
            std::cerr << "DemoGame: Failed to create MainMenuScreen" << std::endl;
        }
        return true;
    }

    void OnLoadContent() override
    {
        std::cout << "DemoGame: Loading content..." << std::endl;

        // Try to locate a test image
        auto FindAsset = [](const std::string& relative) -> std::optional<std::string> {
            fs::path start = fs::current_path();
            for (int up = 0; up < 6; ++up)
            {
                fs::path p = start;
                for (int i = 0; i < up; ++i) p = p.parent_path();
                p /= relative;
                if (fs::exists(p)) return p.string();
            }
            return std::nullopt;
        };

        auto assetPath = FindAsset("Content/Textures/Resources/Start/Logo.png");
        if (!assetPath)
            assetPath = FindAsset("Content/Textures/Resources/Start/Start.jpg");
        if (!assetPath)
            assetPath = FindAsset("Content/Textures/Resources/Start/Play.png");

        if (assetPath)
        {
            SDL_Renderer* renderer = Platform::GetSDLRenderer();
            m_testTexture = Texture::LoadFromFile(renderer, *assetPath);
            if (m_testTexture)
                std::cout << "DemoGame: Loaded texture: " << *assetPath << std::endl;
            else
                std::cerr << "DemoGame: Failed to load texture: " << *assetPath << std::endl;
        }
        else
        {
            std::cerr << "DemoGame: No test asset found" << std::endl;
        }
    }

    void OnUpdate(float deltaTime) override
    {
        // Update game logic
    }

    void OnDraw() override
    {
        SDL_Renderer* renderer = Platform::GetSDLRenderer();
        if (!renderer)
            return;

        // Draw test texture if loaded
        if (m_testTexture)
        {
            int w = m_testTexture->width();
            int h = m_testTexture->height();
            SDL_Rect dst{ (GetWindowWidth() - w) / 2, (GetWindowHeight() - h) / 2, w, h };
            SpriteBatch batch(renderer);
            batch.Draw(m_testTexture->GetSDLTexture(), nullptr, &dst);
        }

        // Draw FPS info
        // TODO: Render text info (requires font rendering)
    }

    void OnShutdown() override
    {
        std::cout << "DemoGame: Shutting down..." << std::endl;
        m_testTexture.reset();
    }
};

int main()
{
    try
    {
        std::cout << "=== World of the Three Kingdoms - C++ Port ===" << std::endl;
        std::cout << "Version: Debug / SDL2 / MainGame Framework" << std::endl;
        std::cout << std::endl;

        std::cout << "[1/4] Creating game instance..." << std::endl;
        std::cout.flush();
        auto game = std::make_shared<DemoGame>();
        GameManager::MainGame::Current = game;
        std::cout << "[1/4] Game instance created" << std::endl;
        std::cout << std::endl;

        std::cout << "[2/4] Initializing game systems..." << std::endl;
        std::cout.flush();
        if (!game->Initialize())
        {
            std::cerr << "ERROR: Failed to initialize game" << std::endl;
            std::cerr.flush();
            return -1;
        }
        std::cout << "[2/4] Game initialization complete" << std::endl;
        std::cout << std::endl;

        std::cout << "[3/4] Starting main game loop..." << std::endl;
        std::cout << "Controls: ESC to exit, Mouse to interact" << std::endl;
        std::cout << std::endl;
        std::cout.flush();

        int result = game->Run();

        std::cout << std::endl;
        std::cout << "[4/4] Game shutdown complete" << std::endl;
        std::cout << "Game exited with code: " << result << std::endl;
        std::cout.flush();
        return result;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "FATAL ERROR: Unhandled exception: " << ex.what() << std::endl;
        std::cerr.flush();
        return -1;
    }
}