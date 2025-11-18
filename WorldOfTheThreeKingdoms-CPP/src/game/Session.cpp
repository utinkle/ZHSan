#include "Session.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace GameManager {

// Static member initialization
Session* Session::s_instance = nullptr;
std::mutex Session::WorkLock;

GameGlobal::Parameters Session::ParametersBasic;
GameGlobal::Parameters Session::ParametersTemp;
GameGlobal::GlobalVariables Session::GlobalVariablesBasic;
GameGlobal::GlobalVariables Session::GlobalVariablesTemp;

std::string Session::RealResolution = "";

Session& Session::Instance()
{
    if (!s_instance)
    {
        s_instance = new Session();
    }
    return *s_instance;
}

GameGlobal::Parameters& Session::GetParameters()
{
    auto& instance = Instance();
    if (instance.Scenario == nullptr)
    {
        return ParametersTemp;
    }
    else
    {
        // When scenario is available, prefer scenario parameters if available
        // For now return Basic until scenario parameters are implemented
        return ParametersBasic;
    }
}

GameGlobal::GlobalVariables& Session::GetGlobalVariables()
{
    auto& instance = Instance();
    if (instance.Scenario == nullptr)
    {
        return GlobalVariablesTemp;
    }
    else
    {
        // When scenario is available, prefer scenario variables if available
        // For now return Basic until scenario variables are implemented
        return GlobalVariablesBasic;
    }
}

int Session::GetResolutionX()
{
    std::string resolution = GetResolution();
    if (resolution.empty() || resolution.find('*') == std::string::npos)
    {
        return 0;
    }

    size_t sep = resolution.find('*');
    std::string xStr = resolution.substr(0, sep);
    xStr.erase(0, xStr.find_first_not_of(" \t\r\n"));
    xStr.erase(xStr.find_last_not_of(" \t\r\n") + 1);

    try
    {
        return std::stoi(xStr);
    }
    catch (...)
    {
        return 0;
    }
}

int Session::GetResolutionY()
{
    std::string resolution = GetResolution();
    if (resolution.empty() || resolution.find('*') == std::string::npos)
    {
        return 0;
    }

    size_t sep = resolution.find('*');
    std::string yStr = resolution.substr(sep + 1);
    yStr.erase(0, yStr.find_first_not_of(" \t\r\n"));
    yStr.erase(yStr.find_last_not_of(" \t\r\n") + 1);

    try
    {
        return std::stoi(yStr);
    }
    catch (...)
    {
        return 0;
    }
}

std::string Session::GetResolution()
{
    if (GameGlobal::Setting::Current)
    {
        return GameGlobal::Setting::Current->Resolution;
    }
    return "";
}

void Session::SetResolution(const std::string& value)
{
    if (value.empty() || value.find('*') == std::string::npos)
    {
        return;
    }

    if (GameGlobal::Setting::Current)
    {
        GameGlobal::Setting::Current->Resolution = value;
    }
}

void Session::Init()
{
    std::cout << "Session::Init - Initializing game session" << std::endl;
    
    // Ensure singleton is created
    auto& instance = Instance();
    (void)instance;  // Mark as used to suppress warnings

    // TODO: Load common data, fonts, input gestures from files
    // TODO: Initialize difficulty settings from parameters
}

void Session::LoadContent()
{
    std::cout << "Session::LoadContent - Loading game content" << std::endl;
}

void Session::LoadFont(const std::string& language)
{
    std::cout << "Session::LoadFont - Loading font for language: " << language << std::endl;

    if (language == "cn" || language == "simplified")
    {
        std::cout << "  Loading Chinese font: FZLB_GBK.TTF" << std::endl;
    }
    else
    {
        std::cout << "  Loading English font: JDFGY.TTF" << std::endl;
    }

    std::lock_guard<std::mutex> lock(WorkLock);
}

void Session::ChangeDisplay(bool setScale)
{
    std::cout << "Session::ChangeDisplay - setScale: " << (setScale ? "true" : "false") << std::endl;

    std::string resolution = GetResolution();
    if (resolution.empty())
    {
        resolution = "1024*768";
        SetResolution(resolution);
    }

    int width = GetResolutionX();
    int height = GetResolutionY();

    std::cout << "  Resolution: " << width << "x" << height << std::endl;
}

void Session::Clear()
{
    if (Scenario)
    {
        Scenario = nullptr;
    }
}

void Session::StartScenario(std::shared_ptr<GameObjects::GameScenario> scenario, bool save)
{
    std::cout << "Session::StartScenario - Starting scenario" << std::endl;
}

void Session::PlayMusic(const std::string& category)
{
    std::cout << "Session::PlayMusic - Playing music from category: " << category << std::endl;
}

void Session::StopMusic()
{
    std::cout << "Session::StopMusic - Stopping music" << std::endl;
}

} // namespace GameManager
