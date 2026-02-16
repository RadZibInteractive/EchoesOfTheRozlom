#include "AbilitySystem/Abilities/EotRGA_Run.h"
#include "FrameworkBase/EotRGameplayTags.h"
#include "AbilitySystemComponent.h"

UEotRGA_Run::UEotRGA_Run()
{
	FGameplayTagContainer Tags = GetAssetTags();
	Tags.AddTag(EotRGameplayTags::Ability_Movement_Run);
	SetAssetTags(Tags);

	ActivationBlockedTags.AddTag(EotRGameplayTags::State_Movement_Crouching);
	ActivationBlockedTags.AddTag(EotRGameplayTags::State_Movement_Sprinting);
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

	if (ASC->HasMatchingGameplayTag(EotRGameplayTags::State_Movement_Running))
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	ASC->AddLooseGameplayTag(EotRGameplayTags::State_Movement_Running);
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

	ASC->RemoveLooseGameplayTag(EotRGameplayTags::State_Movement_Running);

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}