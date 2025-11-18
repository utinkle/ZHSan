#pragma once

#include "screen/Screen.h"
#include "ui/Panel.h"
#include "ui/Button.h"
#include <memory>

namespace WO3K {

class MainMenuScreen : public Screen {
public:
    MainMenuScreen() = default;
    ~MainMenuScreen() override = default;

    void Initialize() override;
    void OnEnter() override;
    void Update(double deltaTime) override;
    void Draw() override;
    bool HandleEvent(const void* event) override;

private:
    std::shared_ptr<Panel> rootPanel;
};

} // namespace WO3K
