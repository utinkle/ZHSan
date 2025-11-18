#pragma once

#include <string>
#include <memory>
#include "GlobalVariables.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

namespace GameGlobal {

/// <summary>
/// Setting class holds user settings and preferences.
/// Mirrors C# GameGlobal.Setting (186 lines).
/// </summary>
class Setting {
public:
    std::string UserGuid;
    std::string DeviceID;
    std::string Language = "cn";
    int MusicVolume = 70;
    int SoundVolume = 50;
    std::string DisplayMode = "Window";  // "Window" or "Full"
    std::string Resolution = "1024*768";
    std::string GamerName;
    int SpeedUp = 1;
    bool Chuchangsuiji = false;  // random starting position
    std::string MOD;
    std::string PortraitPack;

    std::shared_ptr<GlobalVariables> GlobalVariablesData;

    Setting();
    ~Setting() = default;

    static std::shared_ptr<Setting> Current;

    /// <summary>
    /// Initialize settings from file (Setting.config)
    /// </summary>
    static void Init(bool prepare);

    /// <summary>
    /// Save settings to file
    /// </summary>
    static void Save();

    /// <summary>
    /// JSON serialization support for nlohmann/json
    /// </summary>
    friend void to_json(json& j, const Setting& s);
    friend void from_json(const json& j, Setting& s);

private:
    static void Prepare();
    static std::string GenerateGUID();
    static std::string GetDeviceID();
};

} // namespace GameGlobal
