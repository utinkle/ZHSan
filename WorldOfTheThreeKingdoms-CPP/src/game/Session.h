#pragma once

#include <memory>
#include <string>
#include <map>
#include <mutex>
#include "GameGlobal/Parameters.h"
#include "GameGlobal/GlobalVariables.h"
#include "GameGlobal/Setting.h"
#include "GameScenario.h"

namespace GameManager {

enum class Difficulty {
    beginner,
    easy,
    normal,
    hard,
    veryhard,
    custom
};

/// <summary>
/// Session class is the main game state manager.
/// It holds the current GameScenario, Parameters, GlobalVariables, and resolution settings.
/// Mirrors C# GameManager.Session (697 lines).
/// </summary>
class Session {
public:
    Session() = default;
    ~Session() = default;

    // Static singleton instance
    static Session& Instance();

    // Properties
    bool IsWorking = false;
    bool LargeContextMenu = false;

    // Game scenario holder
    std::shared_ptr<GameObjects::GameScenario> Scenario;

    // Static game state holders
    static GameGlobal::Parameters ParametersBasic;
    static GameGlobal::Parameters ParametersTemp;
    static GameGlobal::GlobalVariables GlobalVariablesBasic;
    static GameGlobal::GlobalVariables GlobalVariablesTemp;

    /// <summary>
    /// Get the active Parameters (from scenario if available, else temp)
    /// </summary>
    static GameGlobal::Parameters& GetParameters();

    /// <summary>
    /// Get the active GlobalVariables (from scenario if available, else temp)
    /// </summary>
    static GameGlobal::GlobalVariables& GetGlobalVariables();

    /// <summary>
    /// Get resolution X component from resolution string (e.g., "1024*768" -> 1024)
    /// </summary>
    static int GetResolutionX();

    /// <summary>
    /// Get resolution Y component from resolution string (e.g., "1024*768" -> 768)
    /// </summary>
    static int GetResolutionY();

    /// <summary>
    /// Get/set resolution string (e.g., "1024*768")
    /// </summary>
    static std::string GetResolution();
    static void SetResolution(const std::string& value);

    /// <summary>
    /// Real resolution as applied to the graphics device
    /// </summary>
    static std::string RealResolution;

    /// <summary>
    /// Initialize session: load common data, fonts, input gestures
    /// </summary>
    static void Init();

    /// <summary>
    /// Load content from managers
    /// </summary>
    static void LoadContent();

    /// <summary>
    /// Load font based on language setting
    /// </summary>
    static void LoadFont(const std::string& language);

    /// <summary>
    /// Update display/resolution settings
    /// </summary>
    static void ChangeDisplay(bool setScale);

    /// <summary>
    /// Clear game state and unload resources
    /// </summary>
    void Clear();

    /// <summary>
    /// Start a scenario/game session
    /// </summary>
    static void StartScenario(std::shared_ptr<GameObjects::GameScenario> scenario, bool save);

    /// <summary>
    /// Play music from the specified category
    /// </summary>
    static void PlayMusic(const std::string& category);

    /// <summary>
    /// Stop current music playback
    /// </summary>
    static void StopMusic();

    // Thread safety lock for concurrent access (mirrors C# lock(WorkLock))
    static std::mutex WorkLock;

private:
    static Session* s_instance;
};

} // namespace GameManager
