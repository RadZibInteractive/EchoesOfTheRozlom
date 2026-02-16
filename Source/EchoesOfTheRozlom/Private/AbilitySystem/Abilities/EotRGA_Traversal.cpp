#include "AbilitySystem/Abilities/EotRGA_Traversal.h"
#include "FrameworkBase/EotRGameplayTags.h"
#include "GameFramework/Character.h"
#include "Characters/EotRBaseCharacter.h"
#include "Data/Structs/EotRTraversalCheckResult.h"
#include "Animation/AnimMontage.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystemComponent.h"
#include "Components/EotRTraversalComponent.h"
#include "MotionWarpingComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UEotRGA_Traversal::UEotRGA_Traversal()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    FGameplayTagContainer Tags = GetAssetTags();
    Tags.AddTag(EotRGameplayTags::Ability_Movement_Traversal);
    SetAssetTags(Tags);

    ActivationBlockedTags.AddTag(EotRGameplayTags::State_Movement_Traversing);
}

void UEotRGA_Traversal::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
    Super::OnGiveAbility(ActorInfo, Spec);

    if (ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get()))
    {
        Character->GetCharacterMovement()->JumpZVelocity = 500.f;
        Character->GetCharacterMovement()->AirControl = 0.25f;

        UMotionWarpingComponent* WarpComp = nullptr;
        if (!Character->FindComponentByClass<UMotionWarpingComponent>())
        {
            WarpComp = NewObject<UMotionWarpingComponent>(Character);
            WarpComp->RegisterComponent();
        }
        else
        {
            WarpComp = Character->FindComponentByClass<UMotionWarpingComponent>();
        }

        if (!Character->FindComponentByClass<UEotRTraversalComponent>())
        {
            UEotRTraversalComponent* TraversalComp = NewObject<UEotRTraversalComponent>(Character, TraversalComponentClass);
            TraversalComp->RegisterComponent();
            TraversalComp->SetMotionWarpingComponent(WarpComp);
        }
    }
}

void UEotRGA_Traversal::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    AEotRBaseCharacter* EotRCharacter = Cast<AEotRBaseCharacter>(ActorInfo->AvatarActor.Get());
    if (!EotRCharacter)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    if (UEotRTraversalComponent* TraversalComp = EotRCharacter->FindComponentByClass<UEotRTraversalComponent>())
    {
        FEotRTraversalCheckResult Result;
        if (TraversalComp->TryFindTraversal(Result))
        {
            UAnimMontage* MontageToPlay = Result.ChosenMontage;
            if (MontageToPlay)
            {
                TraversalComp->UpdateWarpTargets(Result);

                UAbilityTask_PlayMontageAndWait* Task =
                    UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
                        this,
                        NAME_None,
                        MontageToPlay,
                        Result.PlayRate,
                        NAME_None,
                        true,
                        1.f,
                        Result.StartTime,
                        false
                    );

                if (Task)
                {
                    EotRCharacter->SetAbilityAnimTarget(false);

                    CachedResult = Result;
                        
                    Task->OnBlendedIn.AddDynamic(this, &UEotRGA_Traversal::OnMontageBlendedIn);
                    Task->OnBlendOut.AddDynamic(this, &UEotRGA_Traversal::OnMontageFinished);
                    Task->OnInterrupted.AddDynamic(this, &UEotRGA_Traversal::OnMontageFinished);
                    Task->OnCancelled.AddDynamic(this, &UEotRGA_Traversal::OnMontageFinished);

                    Task->ReadyForActivation();

                    return;
                }
            }
        }
    }

    EotRCharacter->Jump();

    EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
}

void UEotRGA_Traversal::CancelAbility(
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

    ASC->RemoveLooseGameplayTag(EotRGameplayTags::State_Movement_Traversing);

    ACharacter* Character = Cast<ACharacter>(CurrentActorInfo->AvatarActor.Get());
    if (!Character)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    if (UCapsuleComponent* CapsuleComp = Character->GetCapsuleComponent())
    {
        if (UPrimitiveComponent* HitComp = CachedResult.HitComponent.Get())
        {
            CapsuleComp->IgnoreComponentWhenMoving(HitComp, false);
        }
    }

    if (UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement())
    {
        if (CachedResult.ActionType != EEotRTraversalActionType::Vault)
        {
            MoveComp->SetMovementMode(MOVE_Walking);
        }

        if (!MoveComp->bOrientRotationToMovement)
        {
            Character->bUseControllerRotationYaw = true;
        }
    }

    Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UEotRGA_Traversal::OnMontageBlendedIn()
{
    UAbilitySystemComponent* ASC = CurrentActorInfo->AbilitySystemComponent.Get();
    if (ASC)
    {
        ASC->AddLooseGameplayTag(EotRGameplayTags::State_Movement_Traversing);
    }

    if (ACharacter* Character = Cast<ACharacter>(CurrentActorInfo->AvatarActor.Get()))
    {
        if (UCapsuleComponent* CapsuleComp = Character->GetCapsuleComponent())
        {
            if (UPrimitiveComponent* HitComp = CachedResult.HitComponent.Get())
            {
                CapsuleComp->IgnoreComponentWhenMoving(HitComp, true);
            }
        }

        if (UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement())
        {
            MoveComp->SetMovementMode(MOVE_Flying);

            if (!MoveComp->bOrientRotationToMovement)
            {
                Character->bUseControllerRotationYaw = false;
            }
        }
    }
}

void UEotRGA_Traversal::OnMontageFinished()
{
    if (IsEndAbilityValid(CurrentSpecHandle, CurrentActorInfo))
    {
        CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
    }
}