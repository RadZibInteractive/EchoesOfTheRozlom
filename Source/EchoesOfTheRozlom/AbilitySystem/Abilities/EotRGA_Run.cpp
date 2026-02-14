#include "AbilitySystem/Abilities/EotRGA_Run.h"
#include "EotRGameplayTags.h"
#include "AbilitySystemComponent.h"

UEotRGA_Run::UEotRGA_Run()
{
	AbilityTags.AddTag(EotRTags::Ability_Movement_Run);

	ActivationBlockedTags.AddTag(EotRTags::State_Movement_Crouching);
	ActivationBlockedTags.AddTag(EotRTags::State_Movement_Sprinting);
}

void UEotRGA_Run::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	if (!ASC)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (ASC->HasMatchingGameplayTag(EotRTags::State_Movement_Running))
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	ASC->AddLooseGameplayTag(EotRTags::State_Movement_Running);
}

void UEotRGA_Run::CancelAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility
)
{
	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	if (!ASC)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ASC->RemoveLooseGameplayTag(EotRTags::State_Movement_Running);

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}