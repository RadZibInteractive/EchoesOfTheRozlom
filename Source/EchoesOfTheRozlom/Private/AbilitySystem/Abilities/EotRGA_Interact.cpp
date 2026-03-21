// © 2026 RadZib. All rights reserved.

#include "AbilitySystem/Abilities/EotRGA_Interact.h"
#include "FrameworkBase/EotRGameplayTags.h"
#include "Characters/EotRHumanCharacter.h"
#include "Components/EotRInteractionComponent.h"

UEotRGA_Interact::UEotRGA_Interact()
{
    FGameplayTagContainer Tags = GetAssetTags();
    Tags.AddTag(EotRGameplayTags::Ability_Interaction_Interact);
    SetAssetTags(Tags);

    ActivationBlockedTags.AddTag(EotRGameplayTags::State_Movement_Traversing);
    ActivationBlockedTags.AddTag(EotRGameplayTags::State_Movement_Sprinting);
}

void UEotRGA_Interact::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData
)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    AEotRHumanCharacter* EotRCharacter = Cast<AEotRHumanCharacter>(ActorInfo->AvatarActor.Get());
    if (!EotRCharacter)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    if (UEotRInteractionComponent* InteractionComp = EotRCharacter->FindComponentByClass<UEotRInteractionComponent>())
    {
        InteractionComp->TryInteract();
    }

    EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
}