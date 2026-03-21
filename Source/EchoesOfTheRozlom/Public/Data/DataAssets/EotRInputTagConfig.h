// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ForgeDataAsset.h"
#include "GameplayTagContainer.h"
#include "EotRInputTagConfig.generated.h"

class UInputAction;

USTRUCT()
struct ECHOESOFTHEROZLOM_API FEotRTaggedInputAction
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputTag;
};

UCLASS()
class ECHOESOFTHEROZLOM_API UEotRInputTagConfig : public UForgeDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TArray<FEotRTaggedInputAction> TaggedInputActions;
};