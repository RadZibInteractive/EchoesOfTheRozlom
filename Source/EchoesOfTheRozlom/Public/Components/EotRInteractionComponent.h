// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EotRInteractionComponent.generated.h"

UCLASS()
class ECHOESOFTHEROZLOM_API UEotRInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UEotRInteractionComponent();
	void TryInteract();

protected:
	// UActorComponent
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void PerformInteractionTrace(ECollisionChannel TraceChannel = ECC_GameTraceChannel3);

private:
	UPROPERTY()
	TObjectPtr<AActor> FocusedActor = nullptr;
};
