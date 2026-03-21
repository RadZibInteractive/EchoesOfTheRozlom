// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/Enums/EotRTraversalActionType.h"
#include "EotRTraversalCheckResult.generated.h"

USTRUCT(BlueprintType)
struct FEotRTraversalCheckResult
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEotRTraversalActionType ActionType = EEotRTraversalActionType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool HasFrontLedge = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector FrontLedgeLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector FrontLedgeNormal = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool HasBackLedge = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector BackLedgeLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector BackLedgeNormal = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool HasBackFloor = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector BackFloorLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ObstacleHeight = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ObstacleDepth = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BackLedgeHeight = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UPrimitiveComponent> HitComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAnimMontage> ChosenMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StartTime = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PlayRate = 1.f;
};