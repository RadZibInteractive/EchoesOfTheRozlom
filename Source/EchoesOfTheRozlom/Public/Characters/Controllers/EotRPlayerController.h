#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EotRPlayerController.generated.h"

UCLASS()
class ECHOESOFTHEROZLOM_API AEotRPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	// APlayerController
	virtual void InitInputSystem() override;
	virtual void UpdateRotation(float DeltaTime) override;
};