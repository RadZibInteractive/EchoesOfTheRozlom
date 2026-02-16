#pragma once

#include "CoreMinimal.h"
#include "Data/Enums/EotRTraversalActionType.h"
#include "EotRTraversalCheckResult.generated.h"

USTRUCT(BlueprintType)
struct FEotRTraversalCheckResult
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "ActionType"))
	EEotRTraversalActionType ActionType = EEotRTraversalActionType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "HasFrontLedge"))
	bool HasFrontLedge = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "FrontLedgeLocation"))
	FVector FrontLedgeLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "FrontLedgeNormal"))
	FVector FrontLedgeNormal = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "HasBackLedge"))
	bool HasBackLedge = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "BackLedgeLocation"))
	FVector BackLedgeLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "BackLedgeNormal"))
	FVector BackLedgeNormal = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "HasBackFloor"))
	bool HasBackFloor = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "BackFloorLocation"))
	FVector BackFloorLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "ObstacleHeight"))
	float ObstacleHeight = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "ObstacleDepth"))
	float ObstacleDepth = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "BackLedgeHeight"))
	float BackLedgeHeight = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "HitComponent"))
	TObjectPtr<UPrimitiveComponent> HitComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "ChosenMontage"))
	TObjectPtr<UAnimMontage> ChosenMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "StartTime"))
	float StartTime = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "PlayRate"))
	float PlayRate = 1.f;
};