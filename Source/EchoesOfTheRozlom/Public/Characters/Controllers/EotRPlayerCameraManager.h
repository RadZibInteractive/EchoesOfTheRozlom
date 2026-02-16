#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "EotRPlayerCameraManager.generated.h"

UCLASS()
class ECHOESOFTHEROZLOM_API AEotRPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

protected:
	// APlayerCameraManager
	virtual void SetViewTarget(AActor* NewViewTarget, FViewTargetTransitionParams TransitionParams) override;

	virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;
	virtual void UpdateViewTargetInternal(FTViewTarget& OutVT, float DeltaTime);
};