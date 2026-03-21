// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EotRTraversalCheckInput.generated.h"

USTRUCT(BlueprintType)
struct FEotRTraversalCheckInput
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector TraceForwardDirection = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TraceForwardDistance = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector TraceOriginOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector TraceEndOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TraceRadius = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TraceHalfHeight = 0.f;
};