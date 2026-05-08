// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Data/ForgeTypes.h"
#include "ForgeNPCProfile.generated.h"

USTRUCT(BlueprintType)
struct FORGECORE_API FForgeAggroRules
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0.0"))
	float SightRadius = 1500.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0.0"))
	float HearingRadius = 800.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bAttackOnSight = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bCallForHelp = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AlertCooldownSeconds = 5.f;
};

USTRUCT(BlueprintType)
struct FORGECORE_API FForgeCombatScenario
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName ScenarioId;

	// e.g. "keep distance", "rush", "take cover"
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float HealthThresholdMin = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float HealthThresholdMax = 1.f;
};

UCLASS(BlueprintType)
class FORGECORE_API UForgeNPCProfile : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "NPC|Identity")
	FName ProfileId;

	UPROPERTY(EditDefaultsOnly, Category = "NPC|Identity")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, Category = "NPC|Behavior")
	EForgeNPCArchetype Archetype = EForgeNPCArchetype::Neutral;

	UPROPERTY(EditDefaultsOnly, Category = "NPC|Behavior")
	FForgeAggroRules AggroRules;

	UPROPERTY(EditDefaultsOnly, Category = "NPC|Behavior")
	TArray<FForgeCombatScenario> CombatScenarios;

	UPROPERTY(EditDefaultsOnly, Category = "NPC|Dialogue")
	TSoftObjectPtr<class UForgeDialogueData> DialogueData;

	UPROPERTY(EditDefaultsOnly, Category = "NPC|Dialogue")
	TSoftObjectPtr<class UForgeQuestData> AssociatedQuest;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("ForgeNPCProfile", GetFName());
	}
};
