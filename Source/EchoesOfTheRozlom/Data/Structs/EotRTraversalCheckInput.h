#pragma once

#include "CoreMinimal.h"
#include "EotRTraversalCheckInput.generated.h"

USTRUCT(BlueprintType)
struct FEotRTraversalCheckInput
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "TraceForwardDirection"))
	FVector TraceForwardDirection = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "TraceForwardDistance"))
	float TraceForwardDistance = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "TraceOriginOffset"))
	FVector TraceOriginOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "TraceEndOffset"))
	FVector TraceEndOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "TraceRadius"))
	float TraceRadius = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "TraceHalfHeight"))
	float TraceHalfHeight = 0.f;
};