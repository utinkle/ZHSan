#include "Setting.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <random>
#include <sstream>
#include "../../tools/SimpleSerializer.h"

namespace fs = std::filesystem;

namespace GameGlobal {

std::shared_ptr<Setting> Setting::Current = nullptr;

Setting::Setting()
{
    GlobalVariablesData = std::make_shared<GlobalVariables>();
}

void Setting::Init(bool prepare)
{
    if (Current)
        return;

    std::string filename = "Setting.config";
    
    // Try to load from file
    if (SimpleSerializer::UserFileExists(filename))
    {
        try
        {
            // Read JSON file
            std::string userDir = [&]() {
                #ifdef _WIN32
                    const char* appdata = std::getenv("APPDATA");
                    if (appdata)
                        return std::string(appdata) + "\\WorldOfTheThreeKingdoms\\";
                    else
                        return std::string(".config\\WorldOfTheThreeKingdoms\\");
                #else
                    const char* home = std::getenv("HOME");
                    if (home)
                        return std::string(home) + "/.config/WorldOfTheThreeKingdoms/";
                    else
                        return std::string(".config/WorldOfTheThreeKingdoms/");
                #endif
            }();
            
            std::string filepath = userDir + filename;
            if (fs::exists(filepath))
            {
                std::ifstream file(filepath);
                if (file)
                {
                    std::stringstream buffer;
                    buffer << file.rdbuf();
                    file.close();

                    try
                    {
                        json j = json::parse(buffer.str());
                        Current = std::make_shared<Setting>();

                        // Parse JSON fields
                        if (j.contains("UserGuid") && j["UserGuid"].is_string())
                            Current->UserGuid = j["UserGuid"];
                        if (j.contains("DeviceID") && j["DeviceID"].is_string())
                            Current->DeviceID = j["DeviceID"];
                        if (j.contains("Language") && j["Language"].is_string())
                            Current->Language = j["Language"];
                        if (j.contains("MusicVolume") && j["MusicVolume"].is_number_integer())
                            Current->MusicVolume = j["MusicVolume"];
                        if (j.contains("SoundVolume") && j["SoundVolume"].is_number_integer())
                            Current->SoundVolume = j["SoundVolume"];
                        if (j.contains("DisplayMode") && j["DisplayMode"].is_string())
                            Current->DisplayMode = j["DisplayMode"];
                        if (j.contains("Resolution") && j["Resolution"].is_string())
                            Current->Resolution = j["Resolution"];
                        if (j.contains("GamerName") && j["GamerName"].is_string())
                            Current->GamerName = j["GamerName"];
                        if (j.contains("SpeedUp") && j["SpeedUp"].is_number_integer())
                            Current->SpeedUp = j["SpeedUp"];
                        if (j.contains("Chuchangsuiji") && j["Chuchangsuiji"].is_boolean())
                            Current->Chuchangsuiji = j["Chuchangsuiji"];
                        if (j.contains("MOD") && j["MOD"].is_string())
                            Current->MOD = j["MOD"];
                        if (j.contains("PortraitPack") && j["PortraitPack"].is_string())
                            Current->PortraitPack = j["PortraitPack"];

                        if (prepare)
                        {
                            Prepare();
                            Save();  // Save with any new defaults filled in
                        }
                        return;
                    }
                    catch (const json::exception& ex)
                    {
                        std::cerr << "JSON parse error in Setting.config: " << ex.what() << std::endl;
                    }
                }
            }
        }
        catch (const std::exception& ex)
        {
            std::cerr << "Error loading Setting.config: " << ex.what() << std::endl;
        }
    }

    // If file doesn't exist or failed to load, create new default
    Current = std::make_shared<Setting>();
    if (prepare)
    {
        Prepare();
        Save();
    }
}

void Setting::Save()
{
    if (!Current)
        return;

    try
    {
        std::string userDir = [&]() {
            #ifdef _WIN32
                const char* appdata = std::getenv("APPDATA");
                if (appdata)
                    return std::string(appdata) + "\\WorldOfTheThreeKingdoms\\";
                else
                    return std::string(".config\\WorldOfTheThreeKingdoms\\");
            #else
                const char* home = std::getenv("HOME");
                if (home)
                    return std::string(home) + "/.config/WorldOfTheThreeKingdoms/";
                else
                    return std::string(".config/WorldOfTheThreeKingdoms/");
            #endif
        }();

        if (!fs::exists(userDir))
        {
            fs::create_directories(userDir);
        }

        std::string filepath = userDir + "Setting.config";

        json j;
        j["UserGuid"] = Current->UserGuid;
        j["DeviceID"] = Current->DeviceID;
        j["Language"] = Current->Language;
        j["MusicVolume"] = Current->MusicVolume;
        j["SoundVolume"] = Current->SoundVolume;
        j["DisplayMode"] = Current->DisplayMode;
        j["Resolution"] = Current->Resolution;
        j["GamerName"] = Current->GamerName;
        j["SpeedUp"] = Current->SpeedUp;
        j["Chuchangsuiji"] = Current->Chuchangsuiji;
        j["MOD"] = Current->MOD;
        j["PortraitPack"] = Current->PortraitPack;

        std::ofstream file(filepath);
        if (file)
        {
            file << j.dump(2);  // Pretty print with 2-space indent
            file.close();
            std::cout << "Setting saved to: " << filepath << std::endl;
        }
        else
        {
            std::cerr << "Failed to open Setting.config for writing: " << filepath << std::endl;
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Error saving Setting.config: " << ex.what() << std::endl;
    }
}

void Setting::Prepare()
{
    if (!Current)
        return;

    if (Current->UserGuid.empty())
    {
        Current->UserGuid = GenerateGUID();
    }

    if (Current->DeviceID.empty())
    {
        Current->DeviceID = GetDeviceID();
    }

    if (Current->DisplayMode.empty())
    {
        Current->DisplayMode = "Window";
    }

    if (Current->Language.empty())
    {
        Current->Language = "cn";
    }

    if (Current->MusicVolume <= 0)
    {
        Current->MusicVolume = 70;
    }

    if (Current->SoundVolume <= 0)
    {
        Current->SoundVolume = 50;
    }

    if (Current->SpeedUp <= 0)
    {
        Current->SpeedUp = 1;
    }

    // Set default global variables if not set
    if (!Current->GlobalVariablesData)
    {
        Current->GlobalVariablesData = std::make_shared<GlobalVariables>();
    }
}

std::string Setting::GenerateGUID()
{
    // Simple GUID generation (not RFC 4122 compliant, but sufficient for this use)
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);

    const char* hex = "0123456789abcdef";
    std::string guid;
    guid.reserve(36);

    for (int i = 0; i < 36; ++i)
    {
        if (i == 8 || i == 13 || i == 18 || i == 23)
            guid += '-';
        else
            guid += hex[dis(gen)];
    }

    return guid;
}

std::string Setting::GetDeviceID()
{
    // On Windows: Get from registry or create a unique ID
    // For now, use a combination of system info
    #ifdef _WIN32
        // Could use Windows API to get hardware info
        // For simplicity, generate a unique ID
        return "PC-" + GenerateGUID().substr(0, 8);
    #else
        // On Linux/Mac: Use system information
        return "DEV-" + GenerateGUID().substr(0, 8);
    #endif
}

// JSON serialization helpers
void to_json(json& j, const Setting& s)
{
    j = json{
        {"UserGuid", s.UserGuid},
        {"DeviceID", s.DeviceID},
        {"Language", s.Language},
        {"MusicVolume", s.MusicVolume},
        {"SoundVolume", s.SoundVolume},
        {"DisplayMode", s.DisplayMode},
        {"Resolution", s.Resolution},
        {"GamerName", s.GamerName},
        {"SpeedUp", s.SpeedUp},
        {"Chuchangsuiji", s.Chuchangsuiji},
        {"MOD", s.MOD},
        {"PortraitPack", s.PortraitPack}
    };
}

void from_json(const json& j, Setting& s)
{
    if (j.contains("UserGuid")) j.at("UserGuid").get_to(s.UserGuid);
    if (j.contains("DeviceID")) j.at("DeviceID").get_to(s.DeviceID);
    if (j.contains("Language")) j.at("Language").get_to(s.Language);
    if (j.contains("MusicVolume")) j.at("MusicVolume").get_to(s.MusicVolume);
    if (j.contains("SoundVolume")) j.at("SoundVolume").get_to(s.SoundVolume);
    if (j.contains("DisplayMode")) j.at("DisplayMode").get_to(s.DisplayMode);
    if (j.contains("Resolution")) j.at("Resolution").get_to(s.Resolution);
    if (j.contains("GamerName")) j.at("GamerName").get_to(s.GamerName);
    if (j.contains("SpeedUp")) j.at("SpeedUp").get_to(s.SpeedUp);
    if (j.contains("Chuchangsuiji")) j.at("Chuchangsuiji").get_to(s.Chuchangsuiji);
    if (j.contains("MOD")) j.at("MOD").get_to(s.MOD);
    if (j.contains("PortraitPack")) j.at("PortraitPack").get_to(s.PortraitPack);
}

} // namespace GameGlobal
