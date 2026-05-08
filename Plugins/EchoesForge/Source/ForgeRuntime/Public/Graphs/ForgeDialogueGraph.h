// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Graphs/ForgeGraphBase.h"
#include "Data/ForgeDialogueData.h"
#include "ForgeDialogueGraph.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogueNodeReached, const FForgeDialogueNode&, Node);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDialogueFinished);

// Drives traversal of a UForgeDialogueData tree.
// Evaluates FForgeDialogueCheck conditions to filter available responses.
UCLASS(BlueprintType)
class FORGERUNTIME_API UForgeDialogueGraph : public UForgeGraphBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Forge|Dialogue")
	FOnDialogueNodeReached OnNodeReached;

	UPROPERTY(BlueprintAssignable, Category = "Forge|Dialogue")
	FOnDialogueFinished OnDialogueFinished;

	UFUNCTION(BlueprintCallable, Category = "Forge|Dialogue")
	void StartDialogue(UForgeDialogueData* InData);

	// Pick a player response by its NodeId (must be in current node's ResponseNodeIds)
	UFUNCTION(BlueprintCallable, Category = "Forge|Dialogue")
	void SelectResponse(FName ResponseNodeId);

	// Returns response nodes that pass all Condition checks for the current context
	UFUNCTION(BlueprintPure, Category = "Forge|Dialogue")
	TArray<FForgeDialogueNode> GetAvailableResponses() const;

	UFUNCTION(BlueprintPure, Category = "Forge|Dialogue")
	bool IsDialogueActive() const { return bIsActive; }

	virtual void Initialize() override;

private:
	UPROPERTY()
	TObjectPtr<UForgeDialogueData> DialogueData;

	bool bIsActive = false;

	void AdvanceTo(FName NodeId);
	bool EvaluateConditions(const TArray<FForgeDialogueCheck>& Conditions) const;
};
