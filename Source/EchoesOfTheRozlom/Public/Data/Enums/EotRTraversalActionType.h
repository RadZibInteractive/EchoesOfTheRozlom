// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EotRTraversalActionType.generated.h"

UENUM(BlueprintType)
enum class EEotRTraversalActionType : uint8
{
	None     UMETA(DisplayName = "None"),
	Hurdle   UMETA(DisplayName = "Hurdle"),
	Vault    UMETA(DisplayName = "Vault"),
	Mantle   UMETA(DisplayName = "Mantle")
};