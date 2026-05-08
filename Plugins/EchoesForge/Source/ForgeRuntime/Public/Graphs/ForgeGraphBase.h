// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ForgeGraphBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnForgeGraphEvent, FName, EventId);

// Base class for all Forge graph runtimes (Quest, Logic, Dialogue).
// Implements the "Data over Code" principle: graphs are executed from pure data assets,
// never from user-accessible script callbacks, keeping the sandbox safe.
UCLASS(Abstract, BlueprintType)
class FORGERUNTIME_API UForgeGraphBase : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Forge|Graph")
	FOnForgeGraphEvent OnGraphEvent;

	UFUNCTION(BlueprintCallable, Category = "Forge|Graph")
	virtual void Initialize() {}

	UFUNCTION(BlueprintCallable, Category = "Forge|Graph")
	virtual void Tick(float DeltaTime) {}

	UFUNCTION(BlueprintPure, Category = "Forge|Graph")
	FName GetActiveNodeId() const { return ActiveNodeId; }

protected:
	UPROPERTY()
	FName ActiveNodeId;

	void SetActiveNode(FName NodeId);
	void BroadcastEvent(FName EventId);
};
