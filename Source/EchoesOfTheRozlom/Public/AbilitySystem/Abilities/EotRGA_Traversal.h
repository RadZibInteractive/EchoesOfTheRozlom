#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Components/EotRTraversalComponent.h"
#include "EotRGA_Traversal.generated.h"

UCLASS(Abstract)
class ECHOESOFTHEROZLOM_API UEotRGA_Traversal : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UEotRGA_Traversal();

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

	// Blueprint implementation
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Default")
	TSubclassOf<UEotRTraversalComponent> TraversalComponentClass;

private:
	UFUNCTION()
	void OnMontageBlendedIn();

	UFUNCTION()
	void OnMontageFinished();
};