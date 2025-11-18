#pragma once

#include "screen/Screen.h"
#include <chrono>

namespace WO3K {

class LoadingScreen : public Screen {
public:
    LoadingScreen() = default;
    ~LoadingScreen() override = default;

    void Initialize() override;
    void Update(double deltaTime) override;
    void Draw() override;

private:
    std::chrono::high_resolution_clock::time_point m_start;
    double m_elapsed = 0.0;
};

} // namespace WO3K
