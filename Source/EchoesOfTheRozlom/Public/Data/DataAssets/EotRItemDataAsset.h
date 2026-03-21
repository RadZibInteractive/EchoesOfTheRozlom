// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ForgeDataAsset.h"
#include "GameplayTagContainer.h"

class UStaticMesh;
class UTexture2D;

#include "EotRItemDataAsset.generated.h"

UCLASS()
class ECHOESOFTHEROZLOM_API UEotRItemDataAsset : public UForgeDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Identity",
		meta = (Tooltip = "Tag to identify this exact item, e.g. for particular quest", Categories = "Item.ID"))
	FGameplayTag ItemID;

	UPROPERTY(EditAnywhere, Category = "View",
		meta = (Tooltip = "World mesh to be picked up"))
	TObjectPtr<UStaticMesh> PickupMesh = nullptr;

	UPROPERTY(EditAnywhere, Category = "UI",
		meta = (Tooltip = "Name of the item displayed in inventory"))
	FText DisplayName;

	UPROPERTY(EditAnywhere, Category = "UI",
		meta = (Tooltip = "Description of the item displayed under the name in inventory"))
	FText Description;

	UPROPERTY(EditAnywhere, Category = "UI",
		meta = (Tooltip = "Image of the item displayed inside slot in inventory"))
	TObjectPtr<UTexture2D> Icon = nullptr;

	UPROPERTY(EditAnywhere, Category = "Storage",
		meta = (Tooltip = "Max amount of item copies one inventory slot can hold", ClampMin = "1", UIMin = "1"))
	int32 MaxStackCount = 1;

	UPROPERTY(EditAnywhere, Category = "Storage",
		meta = (Tooltip = "Amount of weight 1 copy of item occupies", ClampMin = "0.0", UIMin = "0.0"))
	float Weight = 0.f;

	UPROPERTY(EditAnywhere, Category = "Storage",
		meta = (Tooltip = "Item dimension inside inventory", ClampMin = "1", UIMin = "1"))
	FIntPoint GridSize = FIntPoint(1, 1);
};
