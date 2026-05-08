// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/ForgeTypes.h"

struct FForgeValidationResult
{
	EForgeValidationStatus Status  = EForgeValidationStatus::Valid;
	FString                Actor;   // actor label for context
	FString                Message;
};

class FORGEEDITOR_API FForgeValidator
{
public:
	// Run all checks on the active editor world. Returns at least one entry.
	static TArray<FForgeValidationResult> ValidateWorld(UWorld* World);

private:
	// Per-type checks — each appends any issues it finds to Out.
	static void CheckNPCActors    (UWorld* World, TArray<FForgeValidationResult>& Out);
	static void CheckAnomalyActors(UWorld* World, TArray<FForgeValidationResult>& Out);
	static void CheckEnemyActors  (UWorld* World, TArray<FForgeValidationResult>& Out);
	static void CheckPropActors   (UWorld* World, TArray<FForgeValidationResult>& Out);
};
