#pragma once

#include "screen/Screen.h"

namespace WO3K {

class GameScreen : public Screen {
public:
    GameScreen() = default;
    ~GameScreen() override = default;

    void Initialize() override {}
    void Update(double deltaTime) override {}
    void Draw() override;
};

} // namespace WO3K
