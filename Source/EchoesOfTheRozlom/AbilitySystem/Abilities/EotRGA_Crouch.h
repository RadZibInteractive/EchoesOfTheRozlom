#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "EotRGA_Crouch.generated.h"

UCLASS()
class ECHOESOFTHEROZLOM_API UEotRGA_Crouch : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UEotRGA_Crouch();

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

protected:
	// UGameplayAbility
	virtual void OnGiveAbility(
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilitySpec& Spec
	) override;
};