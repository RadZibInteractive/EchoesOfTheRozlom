// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "EotRInteractableInterface.generated.h"

UINTERFACE(MinimalAPI)
class UEotRInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

class ECHOESOFTHEROZLOM_API IEotRInteractableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	void OnFocusGained();

	UFUNCTION(BlueprintNativeEvent)
	void OnFocusLost();

	UFUNCTION(BlueprintNativeEvent)
	void Interact(AActor* Interactor);
};
