// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Graphs/ForgeGraphBase.h"
#include "Data/ForgeTypes.h"
#include "Data/ForgeQuestData.h"
#include "ForgeQuestGraph.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnQuestStateChanged, FName, QuestId, EForgeQuestState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnObjectiveUpdated, FName, ObjectiveId, int32, NewCount);

// State machine that drives a quest through: Inactive -> Active -> InProgress -> Completed/Failed.
UCLASS(BlueprintType)
class FORGERUNTIME_API UForgeQuestGraph : public UForgeGraphBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Forge|Quest")
	FOnQuestStateChanged OnQuestStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "Forge|Quest")
	FOnObjectiveUpdated OnObjectiveUpdated;

	UFUNCTION(BlueprintCallable, Category = "Forge|Quest")
	void SetQuestData(UForgeQuestData* InData);

	UFUNCTION(BlueprintCallable, Category = "Forge|Quest")
	void ActivateQuest();

	UFUNCTION(BlueprintCallable, Category = "Forge|Quest")
	void FailQuest();

	UFUNCTION(BlueprintCallable, Category = "Forge|Quest")
	void ProgressObjective(FName ObjectiveId, int32 Amount = 1);

	UFUNCTION(BlueprintPure, Category = "Forge|Quest")
	EForgeQuestState GetQuestState() const { return CurrentState; }

	UFUNCTION(BlueprintPure, Category = "Forge|Quest")
	bool IsAllObjectivesComplete() const;

	virtual void Initialize() override;

private:
	UPROPERTY()
	TObjectPtr<UForgeQuestData> QuestData;

	UPROPERTY()
	EForgeQuestState CurrentState = EForgeQuestState::Inactive;

	// Runtime objective progress (mirrors QuestData.Objectives counts)
	TMap<FName, int32> ObjectiveProgress;

	void TransitionTo(EForgeQuestState NewState);
	void CheckCompletion();
};
