#pragma once

#include "CoreMinimal.h"
#include "EotRMovementGait.generated.h"

UENUM(BlueprintType)
enum class EEotRMovementGait : uint8
{
	Walk   UMETA(DisplayName = "Walk"),
	Run    UMETA(DisplayName = "Run"),
	Sprint UMETA(DisplayName = "Sprint"),
	Crouch UMETA(DisplayName = "Crouch")
};