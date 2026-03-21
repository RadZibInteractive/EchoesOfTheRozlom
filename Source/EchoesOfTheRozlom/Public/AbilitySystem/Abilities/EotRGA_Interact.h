// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "EotRGA_Interact.generated.h"

UCLASS()
class ECHOESOFTHEROZLOM_API UEotRGA_Interact : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UEotRGA_Interact();

	// UGameplayAbility
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;
};