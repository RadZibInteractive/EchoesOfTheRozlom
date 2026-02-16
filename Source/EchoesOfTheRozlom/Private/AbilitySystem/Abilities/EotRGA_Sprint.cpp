#include "AbilitySystem/Abilities/EotRGA_Sprint.h"
#include "FrameworkBase/EotRGameplayTags.h"
#include "AbilitySystemComponent.h"

UEotRGA_Sprint::UEotRGA_Sprint()
{
	AbilityTags.AddTag(EotRTags::Ability_Movement_Sprint);

	ActivationBlockedTags.AddTag(EotRTags::State_Movement_Crouching);
}

void UEotRGA_Sprint::ActivateAbility(
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

	if (ASC->HasMatchingGameplayTag(EotRTags::State_Movement_Sprinting))
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	if (!ASC->HasMatchingGameplayTag(EotRTags::State_Movement_Running))
	{
		ASC->AddLooseGameplayTag(EotRTags::State_Movement_Running);
		ASC->AddLooseGameplayTag(EotRTags::State_Movement_Run_Enabled);
	}

	ASC->AddLooseGameplayTag(EotRTags::State_Movement_Sprinting);
}

void UEotRGA_Sprint::CancelAbility(
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

	if (ASC->HasMatchingGameplayTag(EotRTags::State_Movement_Run_Enabled))
	{
		ASC->RemoveLooseGameplayTag(EotRTags::State_Movement_Running);
		ASC->RemoveLooseGameplayTag(EotRTags::State_Movement_Run_Enabled);
	}

	ASC->RemoveLooseGameplayTag(EotRTags::State_Movement_Sprinting);

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}
