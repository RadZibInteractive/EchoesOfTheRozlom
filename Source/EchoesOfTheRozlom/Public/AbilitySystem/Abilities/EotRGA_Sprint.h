#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "EotRGA_Sprint.generated.h"

UCLASS()
class ECHOESOFTHEROZLOM_API UEotRGA_Sprint : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UEotRGA_Sprint();

	// UGameplayAbility
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	virtual void CancelAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateCancelAbility
	) override;
};