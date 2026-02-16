// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ForgeDataAsset.generated.h"

UCLASS(Abstract)
class DAFORGERUNTIME_API UForgeDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Forge")
	FName AssetId;

	UPROPERTY(EditDefaultsOnly, Category = "Forge", meta = (DisplayThumbnail = "true"))
	TSoftObjectPtr<UTexture2D> DisplayIcon;

	UPROPERTY(EditDefaultsOnly, Category = "Forge")
	FString DisplayTitle;

};
