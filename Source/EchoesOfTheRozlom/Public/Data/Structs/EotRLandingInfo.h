// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EotRLandingInfo.generated.h"

USTRUCT(BlueprintType)
struct FEotRLandingInfo
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bJustLanded = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector LandVelocity = FVector::ZeroVector;
};