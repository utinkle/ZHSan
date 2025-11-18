#pragma once

#include <string>
#include <memory>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <sstream>
#include "nlohmann/json.hpp"

namespace fs = std::filesystem;
using json = nlohmann::json;

/// <summary>
/// SimpleSerializer provides JSON and XML serialization/deserialization utilities.
/// Mirrors C# GameManager.Tools.SimpleSerializer functionality.
/// </summary>
class SimpleSerializer {
public:
    /// <summary>
    /// Serialize an object to JSON string using nlohmann/json.
    /// For now, we'll use a template specialization pattern for common types.
    /// </summary>
    template<typename T>
    static std::string SerializeJson(const T& obj, bool indented = false)
    {
        try
        {
            json j = obj;  // Requires NLOHMANN_DEFINE_TYPE_INTRUSIVE or NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE
            return j.dump(indented ? 2 : -1);
        }
        catch (const std::exception& ex)
        {
            std::cerr << "SerializeJson error: " << ex.what() << std::endl;
            return "";
        }
    }

    /// <summary>
    /// Deserialize JSON string to object.
    /// </summary>
    template<typename T>
    static T* DeserializeJson(const std::string& jsonStr)
    {
        try
        {
            json j = json::parse(jsonStr);
            return new T(j.get<T>());
        }
        catch (const std::exception& ex)
        {
            std::cerr << "DeserializeJson error: " << ex.what() << std::endl;
            return nullptr;
        }
    }

    /// <summary>
    /// Serialize object to JSON file.
    /// </summary>
    template<typename T>
    static bool SerializeJsonFile(const T& obj, const std::string& filename, bool fullPathProvided = false)
    {
        try
        {
            std::string filepath = fullPathProvided ? filename : GetUserFilePath(filename);
            std::string json_str = SerializeJson(obj, true);  // indented
            
            // Create directory if needed
            fs::path p(filepath);
            if (p.has_parent_path() && !fs::exists(p.parent_path()))
            {
                fs::create_directories(p.parent_path());
            }

            std::ofstream file(filepath);
            if (!file)
            {
                std::cerr << "Failed to open file for writing: " << filepath << std::endl;
                return false;
            }

            file << json_str;
            file.close();
            return true;
        }
        catch (const std::exception& ex)
        {
            std::cerr << "SerializeJsonFile error: " << ex.what() << std::endl;
            return false;
        }
    }

    /// <summary>
    /// Deserialize JSON from file.
    /// </summary>
    template<typename T>
    static T* DeserializeJsonFile(const std::string& filename, bool isUserFile = true)
    {
        try
        {
            std::string filepath = isUserFile ? GetUserFilePath(filename) : filename;
            
            if (!fs::exists(filepath))
            {
                std::cerr << "File not found: " << filepath << std::endl;
                return nullptr;
            }

            std::ifstream file(filepath);
            if (!file)
            {
                std::cerr << "Failed to open file for reading: " << filepath << std::endl;
                return nullptr;
            }

            std::stringstream buffer;
            buffer << file.rdbuf();
            file.close();

            return DeserializeJson<T>(buffer.str());
        }
        catch (const std::exception& ex)
        {
            std::cerr << "DeserializeJsonFile error: " << ex.what() << std::endl;
            return nullptr;
        }
    }

    /// <summary>
    /// Check if user file exists.
    /// </summary>
    static bool UserFileExists(const std::string& filename)
    {
        return fs::exists(GetUserFilePath(filename));
    }

private:
    /// <summary>
    /// Get the user file path (in user config/documents directory).
    /// On Windows: typically C:\Users\[UserName]\AppData\Roaming\WorldOfTheThreeKingdoms\
    /// On Linux: ~/.config/WorldOfTheThreeKingdoms/
    /// On Mac: ~/Library/Application Support/WorldOfTheThreeKingdoms/
    /// </summary>
    static std::string GetUserFilePath(const std::string& filename)
    {
        static std::string userDir;
        if (userDir.empty())
        {
            fs::path basePath;
            
            #ifdef _WIN32
                const char* appdata = std::getenv("APPDATA");
                if (appdata)
                {
                    basePath = fs::path(appdata) / "WorldOfTheThreeKingdoms";
                }
                else
                {
                    basePath = fs::current_path() / ".config" / "WorldOfTheThreeKingdoms";
                }
            #elif defined(__APPLE__)
                const char* home = std::getenv("HOME");
                if (home)
                {
                    basePath = fs::path(home) / "Library" / "Application Support" / "WorldOfTheThreeKingdoms";
                }
            #else  // Linux and other Unix-like
                const char* home = std::getenv("HOME");
                if (home)
                {
                    basePath = fs::path(home) / ".config" / "WorldOfTheThreeKingdoms";
                }
            #endif

            if (!fs::exists(basePath))
            {
                fs::create_directories(basePath);
            }

            userDir = basePath.string();
        }

        return (fs::path(userDir) / filename).string();
    }
};
