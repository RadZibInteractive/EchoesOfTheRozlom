#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "EotRCameraComponent.generated.h"

UCLASS()
class ECHOESOFTHEROZLOM_API UEotRCameraComponent : public UCameraComponent
{
	GENERATED_BODY()

protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	bool bFirstUpdate = true;
	FVector DefaultLocationOffset = FVector(5.f, 10.f, 0.f);
};