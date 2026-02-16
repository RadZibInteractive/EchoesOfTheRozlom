#pragma once

#include "CoreMinimal.h"
#include "EotRLandingInfo.generated.h"

USTRUCT(BlueprintType)
struct FEotRLandingInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bJustLanded = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector LandVelocity = FVector::ZeroVector;
};