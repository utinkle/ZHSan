#pragma once

#include <map>

namespace GameGlobal {

/// <summary>
/// Parameters class holds all game configuration parameters and difficulty settings.
/// Mirrors C# GameGlobal.Parameters (823+ members).
/// For now, we'll keep essential parameters and lazy-load the rest as needed.
/// </summary>
class Parameters {
public:
    // Game difficulty/balance parameters
    float AIArchitectureDamageRate = 1.0f;
    float AIFoodRate = 1.0f;
    float AIFundRate = 1.0f;
    float AIRecruitmentSpeedRate = 1.0f;
    float AITrainingSpeedRate = 1.0f;
    float AITroopDefenceRate = 1.0f;
    float AITroopOffenceRate = 1.0f;
    float ArchitectureDamageRate = 1.0f;
    int AIAntiStratagem = 0;
    int AIAntiSurround = 0;

    // Economy parameters
    int BuyFoodAgriculture = 500;
    int ChangeCapitalCost = 0x1388;
    int ConvincePersonCost = 200;
    float DefaultPopulationDevelopingRate = 6E-05f;
    int DestroyArchitectureCost = 200;
    int FindTreasureChance = 10;

    // Combat/damage parameters
    float FireDamageScale = 0.5f;
    float FollowedLeaderDefenceRateIncrement = 0.2f;
    float FollowedLeaderOffenceRateIncrement = 0.2f;
    float FoodRate = 1.0f;
    int FoodToFundDivisor = 200;
    float FundRate = 1.0f;
    int FundToFoodMultiple = 50;

    // Additional parameters (can be expanded)
    int GossipArchitectureCost = 200;
    int JailBreakArchitectureCost = 200;
    int InstigateArchitectureCost = 200;
    int InternalFundCost = 5;
    float InternalRate = 1.0f;
    int LearnSkillDays = 30;
    int LearnStuntDays = 60;
    int LearnTitleDays = 90;
    int SearchDays = 10;
    int RecruitmentDomination = 50;
    int RecruitmentFundCost = 20;
    int RecruitmentMorale = 100;
    float RecruitmentRate = 1.0f;
    int RewardPersonCost = 100;
    int SellFoodCommerce = 500;
    int SurroundArchitectureDominationUnit = 2;
    float TrainingRate = 1.0f;
    float TroopDamageRate = 1.0f;

    Parameters() = default;
};

} // namespace GameGlobal
