// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Data/ForgeTypes.h"
#include "ForgeQuestData.generated.h"

UCLASS(BlueprintType)
class FORGECORE_API UForgeQuestData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Quest|Identity")
	FName QuestId;

	UPROPERTY(EditDefaultsOnly, Category = "Quest|Identity")
	FText QuestTitle;

	UPROPERTY(EditDefaultsOnly, Category = "Quest|Identity", meta = (MultiLine = true))
	FText QuestDescription;

	UPROPERTY(EditDefaultsOnly, Category = "Quest|Identity")
	TSoftObjectPtr<UTexture2D> QuestIcon;

	UPROPERTY(EditDefaultsOnly, Category = "Quest|Structure")
	TArray<FForgeObjectiveData> Objectives;

	UPROPERTY(EditDefaultsOnly, Category = "Quest|Structure")
	FForgeRewardData Reward;

	UPROPERTY(EditDefaultsOnly, Category = "Quest|Structure")
	TArray<TSoftObjectPtr<UForgeQuestData>> Prerequisites;

	UPROPERTY(EditDefaultsOnly, Category = "Quest|Balance")
	float EstimatedCompletionMinutes = 10.f;

	UPROPERTY(EditDefaultsOnly, Category = "Quest|Balance")
	int32 MechanicsComplexity = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Quest|Marketplace")
	bool bIsSandbox = false;

	UPROPERTY(EditDefaultsOnly, Category = "Quest|Marketplace")
	float RecommendedPrice = 0.f;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("ForgeQuest", GetFName());
	}
};
