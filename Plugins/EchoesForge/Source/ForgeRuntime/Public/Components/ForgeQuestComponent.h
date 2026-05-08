// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/ForgeTypes.h"
#include "ForgeQuestComponent.generated.h"

class UForgeQuestData;
class UForgeQuestGraph;

// Attached to the player character. Owns and drives all active ForgeQuestGraphs.
UCLASS(ClassGroup = "Forge", meta = (BlueprintSpawnableComponent))
class FORGERUNTIME_API UForgeQuestComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UForgeQuestComponent();

	UFUNCTION(BlueprintCallable, Category = "Forge|Quest")
	void AcceptQuest(UForgeQuestData* QuestData);

	UFUNCTION(BlueprintCallable, Category = "Forge|Quest")
	void ProgressObjective(FName QuestId, FName ObjectiveId, int32 Amount = 1);

	UFUNCTION(BlueprintPure, Category = "Forge|Quest")
	EForgeQuestState GetQuestState(FName QuestId) const;

	UFUNCTION(BlueprintPure, Category = "Forge|Quest")
	bool HasActiveQuests() const { return ActiveGraphs.Num() > 0; }

protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY()
	TMap<FName, TObjectPtr<UForgeQuestGraph>> ActiveGraphs;
};
