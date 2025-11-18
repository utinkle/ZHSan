#pragma once

#include <memory>
#include <chrono>
#include <functional>
#include "Session.h"
#include "GameGlobal/Setting.h"
#include "input/InputManager.h"
#include "screen/ScreenManager.h"
#include "ui/FocusManager.h"

namespace GameManager {

/// <summary>
/// MainGame class orchestrates the entire game loop and system initialization.
/// Mirrors C# WorldOfTheThreeKingdoms.MainGame class.
/// Handles:
/// - Game loop initialization
/// - Update/Draw cycle
/// - All system management (Input, Rendering, Session)
/// - Exception handling and cleanup
/// </summary>
class MainGame {
public:
    static std::shared_ptr<MainGame> Current;

    MainGame();
    virtual ~MainGame();

    /// <summary>
    /// Initialize the game and all systems
    /// </summary>
    bool Initialize();

    /// <summary>
    /// Main game loop - to be called from main()
    /// </summary>
    int Run();

    /// <summary>
    /// Exit the game gracefully
    /// </summary>
    void Exit();

    /// <summary>
    /// Get the current game state
    /// </summary>
    bool IsRunning() const { return m_running; }

    /// <summary>
    /// Get the current frame time (delta time)
    /// </summary>
    float GetDeltaTime() const { return m_deltaTime; }

    /// <summary>
    /// Get frames per second
    /// </summary>
    float GetFPS() const { return m_fps; }

    /// <summary>
    /// Set target FPS (default 60)
    /// </summary>
    void SetTargetFPS(int fps) { m_targetFPS = fps; }

    /// <summary>
    /// Get window width
    /// </summary>
    int GetWindowWidth() const { return m_windowWidth; }

    /// <summary>
    /// Get window height
    /// </summary>
    int GetWindowHeight() const { return m_windowHeight; }

    // Expose screen manager so game implementations can push/pop screens
    std::shared_ptr<WO3K::ScreenManager> GetScreenManager() const { return m_screenManager; }
    void PushScreen(const WO3K::ScreenPtr& screen);

    /// <summary>
    /// Provide a default list of unit names for HUDs when the full GameScenario
    /// data model is not yet implemented. Screens may call this when no
    /// provider callback is set.
    /// </summary>
    virtual std::vector<std::string> GetUnitNames() const;

protected:
    /// <summary>
    /// Called when the game initializes
    /// </summary>
    virtual bool OnInitialize() { return true; }

    /// <summary>
    /// Called once per frame before Update
    /// </summary>
    virtual void OnLoadContent() {}

    /// <summary>
    /// Called once per frame after LoadContent
    /// </summary>
    virtual void OnUpdate(float deltaTime) {}

    /// <summary>
    /// Called once per frame for drawing
    /// </summary>
    virtual void OnDraw() {}

    /// <summary>
    /// Called on shutdown
    /// </summary>
    virtual void OnShutdown() {}

private:
    bool m_running;
    int m_windowWidth;
    int m_windowHeight;
    float m_deltaTime;
    float m_fps;
    int m_targetFPS;
    int m_frameCount;
    std::chrono::high_resolution_clock::time_point m_lastTime;
    std::chrono::high_resolution_clock::time_point m_fpsTimer;

    /// <summary>
    /// Initialize platform layer (SDL2)
    /// </summary>
    bool InitializePlatform();

    /// <summary>
    /// Initialize rendering system
    /// </summary>
    bool InitializeRender();

    /// <summary>
    /// Initialize game systems
    /// </summary>
    bool InitializeGameSystems();

    /// <summary>
    /// Perform one frame's update
    /// </summary>
    void UpdateFrame();

    /// <summary>
    /// Perform one frame's draw
    /// </summary>
    void DrawFrame();

    /// <summary>
    /// Shutdown all systems
    /// </summary>
    void Shutdown();

    /// <summary>
    /// Update FPS counter
    /// </summary>
    void UpdateFPS();

    // Screen manager for UI
    std::shared_ptr<WO3K::ScreenManager> m_screenManager;
};

} // namespace GameManager
