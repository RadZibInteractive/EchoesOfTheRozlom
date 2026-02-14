#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EotRPlayerController.generated.h"

UCLASS()
class ECHOESOFTHEROZLOM_API AEotRPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	// APlayerController
	virtual void InitInputSystem() override;
};
