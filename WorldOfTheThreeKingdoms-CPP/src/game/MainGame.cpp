#include "MainGame.h"
#include "platform/PlatformSDL.h"
#include "render/SpriteBatch.h"
#include "platform/Texture.h"
#include "screen/ScreenManager.h"
#include "ui/FocusManager.h"
#include "ui/FontManager.h"
#include "game/CacheManager.h"
#include "audio/SoundManager.h"
#include <iostream>
#include <thread>
#include <filesystem>

namespace fs = std::filesystem;
namespace GameManager {

std::shared_ptr<MainGame> MainGame::Current = nullptr;

MainGame::MainGame()
    : m_running(false)
    , m_windowWidth(1024)
    , m_windowHeight(768)
    , m_deltaTime(0.0f)
    , m_fps(0.0f)
    , m_targetFPS(60)
    , m_frameCount(0)
{
}

MainGame::~MainGame()
{
    Shutdown();
}

bool MainGame::Initialize()
{
    try
    {
        std::cout << "MainGame: Initializing..." << std::endl;
        std::cout.flush();

        // Step 1: Initialize Setting (loads user preferences and config)
        std::cout << "MainGame: Initializing Settings..." << std::endl;
        std::cout.flush();
        GameGlobal::Setting::Init(true);
        std::cout << "MainGame: Settings initialized" << std::endl;
        std::cout.flush();

        // Step 2: Initialize Session (loads scenario and game data)
        std::cout << "MainGame: Initializing Session..." << std::endl;
        std::cout.flush();
        Session::Init();
        std::cout << "MainGame: Session initialized" << std::endl;
        std::cout.flush();

        // Step 3: Initialize platform layer (SDL2)
        std::cout << "MainGame: Step 3/6 - Initializing platform..." << std::endl;
        std::cout.flush();
        if (!InitializePlatform())
        {
            std::cerr << "MainGame: ERROR - Failed to initialize platform" << std::endl;
            std::cerr.flush();
            return false;
        }

        // Step 4: Initialize rendering system
        std::cout << "MainGame: Step 4/6 - Initializing rendering..." << std::endl;
        std::cout.flush();
        if (!InitializeRender())
        {
            std::cerr << "MainGame: ERROR - Failed to initialize rendering" << std::endl;
            std::cerr.flush();
            return false;
        }

        // Step 5: Initialize game systems
        std::cout << "MainGame: Step 5/6 - Initializing game systems..." << std::endl;
        std::cout.flush();
        if (!InitializeGameSystems())
        {
            std::cerr << "MainGame: ERROR - Failed to initialize game systems" << std::endl;
            std::cerr.flush();
            return false;
        }

        // Step 6: Call user initialization
        std::cout << "MainGame: Step 6/6 - User initialization..." << std::endl;
        std::cout.flush();
        if (!OnInitialize())
        {
            std::cerr << "MainGame: ERROR - OnInitialize failed" << std::endl;
            std::cerr.flush();
            return false;
        }

        std::cout << "MainGame: Initialization complete" << std::endl;
        std::cout.flush();
        m_running = true;
        return true;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "MainGame: Exception during initialization: " << ex.what() << std::endl;
        std::cerr.flush();
        return false;
    }
}

int MainGame::Run()
{
    if (!m_running)
    {
        std::cerr << "MainGame: Game not initialized. Call Initialize() first." << std::endl;
        return -1;
    }

    std::cout << "MainGame: Starting game loop" << std::endl;

    m_lastTime = std::chrono::high_resolution_clock::now();
    m_fpsTimer = m_lastTime;

    // Load content once
    OnLoadContent();

    // Main game loop
    SDL_Event ev;
    int frameSleepMs = 1000 / m_targetFPS;

    while (m_running)
    {
        auto frameStartTime = std::chrono::high_resolution_clock::now();

        // Process SDL events
        while (SDL_PollEvent(&ev))
        {
            if (ev.type == SDL_QUIT)
            {
                m_running = false;
                break;
            }

            // Forward raw event to InputManager for per-event processing
            InputManager::Update(m_deltaTime, &ev);

            // Handle window resize events
            if (ev.type == SDL_WINDOWEVENT && ev.window.event == SDL_WINDOWEVENT_RESIZED) {
                m_windowWidth = ev.window.data1;
                m_windowHeight = ev.window.data2;
                InputManager::SWidth = m_windowWidth;
                InputManager::SHeight = m_windowHeight;
                if (m_screenManager) m_screenManager->OnResize(m_windowWidth, m_windowHeight);
                // continue dispatching the event to UI input as well
            }

            // Dispatch to UI screen manager first (modal dialogs, controls)
            if (m_screenManager && m_screenManager->HandleEvent(&ev)) {
                // event handled by UI
                continue;
            }
        }

        // Update frame
        UpdateFrame();

        // Draw frame
        DrawFrame();

        // Sleep to maintain target FPS
        auto frameEndTime = std::chrono::high_resolution_clock::now();
        auto frameTime = std::chrono::duration_cast<std::chrono::milliseconds>(frameEndTime - frameStartTime).count();
        int sleepTime = static_cast<int>(frameSleepMs - frameTime);
        if (sleepTime > 0)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
        }

        UpdateFPS();
    }

    std::cout << "MainGame: Exiting game loop" << std::endl;
    OnShutdown();
    Shutdown();

    return 0;
}

void MainGame::Exit()
{
    m_running = false;
}

bool MainGame::InitializePlatform()
{
    try
    {
        std::cout << "MainGame: Initializing platform layer..." << std::endl;
        std::cout.flush();

        if (!Platform::Init("WorldOfTheThreeKingdoms", m_windowWidth, m_windowHeight))
        {
            std::cerr << "MainGame: ERROR - Failed to initialize SDL platform" << std::endl;
            std::cerr.flush();
            return false;
        }

        std::cout << "MainGame: Platform initialized (SDL2)" << std::endl;
        std::cout.flush();
        return true;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "MainGame: Exception in InitializePlatform: " << ex.what() << std::endl;
        std::cerr.flush();
        return false;
    }
}

bool MainGame::InitializeRender()
{
    try
    {
        // Rendering is initialized automatically by Platform::Init
        // Just verify window is ready
        SDL_Window* window = Platform::GetSDLWindow();
        SDL_Renderer* renderer = Platform::GetSDLRenderer();

        if (!window || !renderer)
        {
            std::cerr << "MainGame: Renderer or window not available" << std::endl;
            return false;
        }

        std::cout << "MainGame: Rendering system initialized" << std::endl;
        return true;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "MainGame: Exception in InitializeRender: " << ex.what() << std::endl;
        return false;
    }
}

bool MainGame::InitializeGameSystems()
{
    try
    {
        // Initialize input manager
        InputManager::SWidth = m_windowWidth;
        InputManager::SHeight = m_windowHeight;

        // Initialize screen manager for UI
        m_screenManager = std::make_shared<WO3K::ScreenManager>();

        // Initialize font subsystem (if available)
        WO3K::FontManager::Init();

        // Initialize audio subsystem (if available)
        WO3K::SoundManager::Init();
        // Wire master volume from saved settings (0..100 -> 0.0..1.0)
        try {
            if (GameGlobal::Setting::Current) {
                float master = static_cast<float>(GameGlobal::Setting::Current->SoundVolume) / 100.0f;
                WO3K::SoundManager::SetMasterVolume(master);
            }
        } catch(...) {}

        std::cout << "MainGame: Game systems initialized" << std::endl;

        // Register common resource aliases (map overview) from ContentLite if present
        try {
            const std::string neoPath = "ContentLite/Textures/Resources/ditu/_neo7.jpg";
            if (std::filesystem::exists(neoPath)) {
                CacheManager::RegisterTextureAlias("Maps/overview", neoPath);
                CacheManager::RegisterTextureRecord("Maps/overview", { "Maps/overview", CacheType::Live, ".jpg" });
                std::cout << "MainGame: Registered alias Maps/overview -> " << neoPath << std::endl;
            }
        } catch(...) {}

        return true;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "MainGame: Exception in InitializeGameSystems: " << ex.what() << std::endl;
        return false;
    }
}

void MainGame::UpdateFrame()
{
    // Calculate delta time
    auto currentTime = std::chrono::high_resolution_clock::now();
    m_deltaTime = std::chrono::duration<float>(currentTime - m_lastTime).count();
    m_lastTime = currentTime;

    // Limit delta time to prevent spiral of death
    if (m_deltaTime > 0.1f)  // Cap at 100ms
    {
        m_deltaTime = 0.1f;
    }

    // Update input
    InputManager::Update(m_deltaTime);

    // Check for exit key
    if (InputManager::IsBackPressed)
    {
        m_running = false;
    }

    // Call user update
    OnUpdate(m_deltaTime);
}

void MainGame::DrawFrame()
{
    SDL_Renderer* renderer = Platform::GetSDLRenderer();
    if (!renderer)
        return;

    // Clear background
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderClear(renderer);

    // Call user draw
    OnDraw();

    // Present
    SDL_RenderPresent(renderer);
}

void MainGame::Shutdown()
{
    try
    {
        std::cout << "MainGame: Shutting down..." << std::endl;

        // Save settings before exit
        GameGlobal::Setting::Save();

        // Shutdown platform
        Platform::Shutdown();

        std::cout << "MainGame: Shutdown complete" << std::endl;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "MainGame: Exception during shutdown: " << ex.what() << std::endl;
    }
}

void MainGame::UpdateFPS()
{
    m_frameCount++;

    auto currentTime = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - m_fpsTimer).count();

    if (elapsed >= 1000)  // Update FPS every second
    {
        m_fps = m_frameCount * 1000.0f / elapsed;
        m_frameCount = 0;
        m_fpsTimer = currentTime;

        // Optional: Log FPS
        // std::cout << "FPS: " << m_fps << std::endl;
    }
}

void MainGame::PushScreen(const WO3K::ScreenPtr& screen)
{
    if (m_screenManager) m_screenManager->Push(screen);
}

std::vector<std::string> MainGame::GetUnitNames() const
{
    // Default placeholder unit names until GameScenario / GameObjects are implemented
    std::vector<std::string> names;
    for (int i = 0; i < 12; ++i) names.push_back(std::string("武将") + std::to_string(i+1));
    return names;
}

} // namespace GameManager


