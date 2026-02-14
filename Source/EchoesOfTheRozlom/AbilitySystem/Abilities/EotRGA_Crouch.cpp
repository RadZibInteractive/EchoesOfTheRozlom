#include "AbilitySystem/Abilities/EotRGA_Crouch.h"
#include "EotRGameplayTags.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UEotRGA_Crouch::UEotRGA_Crouch()
{
	AbilityTags.AddTag(EotRTags::Ability_Movement_Crouch);

	ActivationBlockedTags.AddTag(EotRTags::State_Movement_Sprinting);
}

void UEotRGA_Crouch::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	if (ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get()))
	{
		Character->GetCharacterMovement()->CrouchedHalfHeight = 60.f;
		Character->GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;
		Character->GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	}
}

void UEotRGA_Crouch::ActivateAbility(
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

	if (ASC->HasMatchingGameplayTag(EotRTags::State_Movement_Crouching))
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	if (ASC->HasMatchingGameplayTag(EotRTags::State_Movement_Running))
	{
		ASC->RemoveLooseGameplayTag(EotRTags::State_Movement_Running);
		ASC->AddLooseGameplayTag(EotRTags::State_Movement_Run_Disabled);
	}
	ASC->AddLooseGameplayTag(EotRTags::State_Movement_Crouching);

	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (Character)
	{
		Character->Crouch();
	}
}

void UEotRGA_Crouch::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	if (!ASC)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (ASC->HasMatchingGameplayTag(EotRTags::State_Movement_Run_Disabled))
	{
		ASC->AddLooseGameplayTag(EotRTags::State_Movement_Running);
		ASC->RemoveLooseGameplayTag(EotRTags::State_Movement_Run_Disabled);
	}

	ASC->RemoveLooseGameplayTag(EotRTags::State_Movement_Crouching);

	ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (Character)
	{
		Character->UnCrouch();
	}

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}