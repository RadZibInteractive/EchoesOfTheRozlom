// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ForgeTypes.generated.h"

UENUM(BlueprintType)
enum class EForgeQuestState : uint8
{
	Inactive,
	Active,
	InProgress,
	Completed,
	Failed
};

UENUM(BlueprintType)
enum class EForgeWeaponTier : uint8
{
	Crafted,
	Foundry,
	Rozlom
};

UENUM(BlueprintType)
enum class EForgeNPCArchetype : uint8
{
	Guard,
	Merchant,
	QuestGiver,
	Enemy,
	Neutral
};

UENUM(BlueprintType)
enum class EForgeAnomalyType : uint8
{
	Gravitational,
	Chemical,
	Electrical,
	Thermal,
	Magnetic
};

UENUM(BlueprintType)
enum class EForgeDamageType : uint8
{
	Physical,
	Fire,
	Electric,
	Chemical,
	Rozlom
};

UENUM(BlueprintType)
enum class EForgeValidationStatus : uint8
{
	Valid,
	Warning,
	Error
};

USTRUCT(BlueprintType)
struct FORGECORE_API FForgeObjectiveData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName ObjectiveId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 RequiredCount = 1;

	UPROPERTY(BlueprintReadOnly)
	int32 CurrentCount = 0;

	bool IsComplete() const { return CurrentCount >= RequiredCount; }
};

USTRUCT(BlueprintType)
struct FORGECORE_API FForgeRewardData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 RZLMAmount = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSoftObjectPtr<class UForgeQuestData>> UnlockedQuests;
};
