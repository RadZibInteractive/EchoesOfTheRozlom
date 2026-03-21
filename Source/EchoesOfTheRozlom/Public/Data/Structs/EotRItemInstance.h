// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"

class UEotRItemDataAsset;

#include "EotRItemInstance.generated.h"

USTRUCT()
struct FEotRItemInstance
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UEotRItemDataAsset> ItemData = nullptr;

	UPROPERTY()
	int32 StackCount = 1;
};