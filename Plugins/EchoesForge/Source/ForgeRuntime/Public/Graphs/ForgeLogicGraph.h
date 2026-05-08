// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Graphs/ForgeGraphBase.h"
#include "ForgeLogicGraph.generated.h"

UENUM(BlueprintType)
enum class EForgeNodeType : uint8
{
	Event,       // Entry point: OnTriggerEnter, OnQuestObjective, OnTimer
	Condition,   // If/else branching
	Action,      // SpawnActor, PlaySound, SetQuestObjective, etc.
	Variable,    // Get/Set a named variable
};

USTRUCT(BlueprintType)
struct FORGERUNTIME_API FForgeLogicNode
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName NodeId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EForgeNodeType NodeType = EForgeNodeType::Action;

	// Safe node set: only whitelisted action names are allowed (no file IO / network)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName ActionName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<FName, FString> Parameters;

	// Ordered output pin connections
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FName> OutputNodeIds;
};

// World-scripting graph. Executes trigger logic, spawns, condition checks.
// Safe Node Set enforced at runtime: unknown ActionNames are silently skipped.
UCLASS(BlueprintType)
class FORGERUNTIME_API UForgeLogicGraph : public UForgeGraphBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Forge|Logic")
	TArray<FForgeLogicNode> Nodes;

	UPROPERTY(EditDefaultsOnly, Category = "Forge|Logic")
	FName EntryNodeId;

	UFUNCTION(BlueprintCallable, Category = "Forge|Logic")
	void TriggerEvent(FName EventNodeId);

	UFUNCTION(BlueprintCallable, Category = "Forge|Logic")
	void SetVariable(FName VarName, const FString& Value);

	UFUNCTION(BlueprintPure, Category = "Forge|Logic")
	FString GetVariable(FName VarName) const;

	virtual void Initialize() override;

private:
	TMap<FName, FString> RuntimeVariables;

	static const TSet<FName>& GetSafeActionNames();

	void ExecuteNode(FName NodeId, int32 Depth = 0);
	bool EvaluateCondition(const FForgeLogicNode& Node) const;
};
