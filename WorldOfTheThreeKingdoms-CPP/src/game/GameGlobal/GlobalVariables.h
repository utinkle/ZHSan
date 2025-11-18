#pragma once

#include <string>
#include <optional>

namespace GameGlobal {

enum class MapLayerKind {
    None = 0,
    // Add more as needed
};

/// <summary>
/// GlobalVariables class holds game state variables and rendering/gameplay constants.
/// Mirrors C# GameGlobal.GlobalVariables (1336+ members).
/// Only the most essential variables are included here; expand as needed during implementation.
/// </summary>
class GlobalVariables {
public:
    // Gameplay flags
    bool WujiangYoukenengDuli = true;
    bool LiangdaoXitong = false;
    bool ShowGrid = false;
    bool AdditionalPersonAvailable = false;
    bool CalculateAverageCostOfTiers = false;
    bool CommonPersonAvailable = true;
    bool DrawMapVeil = true;
    bool DrawTroopAnimation = true;
    bool HintPopulation = true;
    bool HintPopulationUnder1000 = true;
    bool IdealTendencyValid = true;
    bool LoadBackGroundMapTexture = false;
    bool MilitaryKindSpeedValid = true;
    bool MultipleResource = false;
    bool NoHintOnSmallFacility = true;
    bool PlayBattleSound = true;

    // Rendering layer depths
    static constexpr float BackgroundDepthOffset = -1E-05f;
    static constexpr float BackTileAnimationLayerDepth = 0.75f;
    static constexpr float ConmentTextDepth = 0.15f;
    static constexpr float ContextMenuDepth = 0.1f;
    static constexpr float ControlDepthOffset = -0.001f;
    static constexpr float DialogDepth = 0.2f;
    static constexpr float FloatingPartDepth = 0.25f;
    static constexpr float FrameContentDepth = 0.35f;
    static constexpr float FrontTileAnimationLayerDepth = 0.65f;
    static constexpr float GameFrameDepth = 0.4f;
    static constexpr float LayerDepthOffset = -0.01f;
    static constexpr float MapLayerDepth = 0.9f;
    static constexpr float MapVeilLayerDepth = 0.6f;
    static constexpr float MapViewSelectorDepth = 0.18f;
    static constexpr float PersonBubbleDepth = 0.45f;
    static constexpr float MaxDepth = 1.0f;
    static constexpr float MinDepth = 0.0f;
    static constexpr float MinDepthOffset = -1E-06f;
    static constexpr float MovableControlDepthOffset = -0.0002f;

    float ArchitectureLayerDepth = 0.8f;
    float MapScrollSpeed = 0.8f;

    // Numeric parameters
    long FactionRunningTicksLimitInOneFrame = 0x186a0;
    int FastBattleSpeed = 1;
    int MaxCountOfKnownPaths = 0x3e8;
    int MaxTimeOfAnimationFrame = 0x19;
    std::optional<bool> PersonNaturalDeath = true;

    std::string GameDifficulty;

    MapLayerKind CurrentMapLayer = MapLayerKind::None;

    GlobalVariables() = default;
};

} // namespace GameGlobal
