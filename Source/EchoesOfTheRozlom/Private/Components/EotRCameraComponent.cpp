// © 2025 RadZib. All rights reserved.

#include "Components/EotRCameraComponent.h"

void UEotRCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bFirstUpdate)
	{
		bFirstUpdate = false;
		SetRelativeLocation(DefaultLocationOffset);
	}
	else
	{
		FVector NewLocation = FMath::VInterpTo(GetRelativeLocation(), DefaultLocationOffset, DeltaTime, 3.0f);
		SetRelativeLocation(NewLocation);
	}
}