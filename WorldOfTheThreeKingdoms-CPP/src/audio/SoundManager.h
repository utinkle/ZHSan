#pragma once

#include <string>
#include <memory>

namespace WO3K {

class SoundManager {
public:
    // Initialize audio subsystem; returns true if a backend is available
    static bool Init();

    // Set master volume (0.0 - 1.0)
    static void SetMasterVolume(float v);

    // Play a sound by key or relative path. Key is resolved under Content/Sound/ if needed.
    // 'volume' is per-playback multiplier (0.0 - 1.0) multiplied by master volume.
    static void Play(const std::string& key, float volume = 1.0f);

    // Clear cached sounds
    static void Clear();
};

} // namespace WO3K
