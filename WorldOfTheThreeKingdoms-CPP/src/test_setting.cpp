#include <iostream>
#include <memory>
#include "game/GameGlobal/Setting.h"

int main()
{
    try
    {
        std::cout << "=== Test Setting Config Loading ===" << std::endl;
        std::cout << std::endl;

        std::cout << "[1] Initializing Setting..." << std::endl;
        GameGlobal::Setting::Init(true);
        std::cout << "[1] Setting initialized" << std::endl;
        std::cout << std::endl;

        if (GameGlobal::Setting::Current)
        {
            std::cout << "[2] Setting.Current values:" << std::endl;
            std::cout << "    UserGuid: " << GameGlobal::Setting::Current->UserGuid << std::endl;
            std::cout << "    DeviceID: " << GameGlobal::Setting::Current->DeviceID << std::endl;
            std::cout << "    Language: " << GameGlobal::Setting::Current->Language << std::endl;
            std::cout << "    MusicVolume: " << GameGlobal::Setting::Current->MusicVolume << std::endl;
            std::cout << "    SoundVolume: " << GameGlobal::Setting::Current->SoundVolume << std::endl;
            std::cout << "    DisplayMode: " << GameGlobal::Setting::Current->DisplayMode << std::endl;
            std::cout << "    Resolution: " << GameGlobal::Setting::Current->Resolution << std::endl;
            std::cout << "    GamerName: " << GameGlobal::Setting::Current->GamerName << std::endl;
            std::cout << "    SpeedUp: " << GameGlobal::Setting::Current->SpeedUp << std::endl;
            std::cout << "    Chuchangsuiji: " << (GameGlobal::Setting::Current->Chuchangsuiji ? "true" : "false") << std::endl;
            std::cout << "    MOD: " << GameGlobal::Setting::Current->MOD << std::endl;
            std::cout << "    PortraitPack: " << GameGlobal::Setting::Current->PortraitPack << std::endl;
            std::cout << std::endl;

            std::cout << "[3] Saving Setting..." << std::endl;
            GameGlobal::Setting::Save();
            std::cout << "[3] Setting saved successfully" << std::endl;
            std::cout << std::endl;

            std::cout << "SUCCESS: All tests passed!" << std::endl;
            return 0;
        }
        else
        {
            std::cerr << "ERROR: Setting::Current is null" << std::endl;
            return -1;
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << "ERROR: " << ex.what() << std::endl;
        return -1;
    }
}
