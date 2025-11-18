#include "audio/SoundManager.h"
#include "platform/PlatformSDL.h"
#include <iostream>
#include <filesystem>
#include <unordered_map>
#include <algorithm>

#ifdef SDL_MIXER_FOUND
#include <SDL_mixer.h>
#endif

namespace WO3K {

static float g_masterVolume = 1.0f;
#ifdef SDL_MIXER_FOUND
static bool g_mixerInit = false;
static std::unordered_map<std::string, Mix_Chunk*> g_soundCache;

static std::string ResolveSoundPath(const std::string& key) {
    namespace fs = std::filesystem;
    if (fs::exists(key)) return key;
    std::string base = "Content/Sound/" + key;
    if (fs::exists(base)) return base;
    const char* exts[] = { ".wav", ".ogg" };
    for (auto e : exts) {
        std::string p = base + e;
        if (fs::exists(p)) return p;
    }
    return base; // fallback
}

bool SoundManager::Init() {
    if (g_mixerInit) return true;
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
        std::cerr << "SoundManager: Mix_OpenAudio failed: " << Mix_GetError() << std::endl;
        return false;
    }
    g_mixerInit = true;
    return true;
}

void SoundManager::Play(const std::string& key, float volume) {
    if (key.empty()) return;
    if (!g_mixerInit) {
        // try to init once
        if (!Init()) return;
    }

    auto it = g_soundCache.find(key);
    Mix_Chunk* chunk = nullptr;
    if (it != g_soundCache.end()) chunk = it->second;
    else {
        std::string path = ResolveSoundPath(key);
        chunk = Mix_LoadWAV(path.c_str());
        if (!chunk) {
            std::cerr << "SoundManager: Mix_LoadWAV failed for " << path << " : " << Mix_GetError() << std::endl;
            return;
        }
        g_soundCache[key] = chunk;
    }

    // compute volume (0..MIX_MAX_VOLUME)
    int vol = static_cast<int>(MIX_MAX_VOLUME * std::clamp(g_masterVolume * volume, 0.0f, 1.0f));
    Mix_VolumeChunk(chunk, vol);
    Mix_PlayChannel(-1, chunk, 0);
}

void SoundManager::Clear() {
    for (auto &p : g_soundCache) {
        if (p.second) Mix_FreeChunk(p.second);
    }
    g_soundCache.clear();
    if (g_mixerInit) {
        Mix_CloseAudio();
        g_mixerInit = false;
    }
}

#else
// SDL_mixer not present: no-op implementations of Init/Play/Clear will be provided below
#endif

void SoundManager::SetMasterVolume(float v) {
    g_masterVolume = std::clamp(v, 0.0f, 1.0f);
}

} // namespace WO3K
