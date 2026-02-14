#include "Characters/EotRBaseCharacter.h"

#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"

#include "Components/EotRCharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"

AEotRBaseCharacter::AEotRBaseCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UEotRCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	UCapsuleComponent* Capsule = GetCapsuleComponent();
	Capsule->SetCapsuleHalfHeight(86.f);
	Capsule->SetCapsuleRadius(50.f);

	USkeletalMeshComponent* LowerMesh = GetMesh();
	LowerMesh->SetRelativeLocation(FVector(0.f, 0.f, -Capsule->GetUnscaledCapsuleHalfHeight() - 2.f));
	LowerMesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
}

EEotRMovementGait AEotRBaseCharacter::GetLastMovementGait() const
{
	if (const UEotRCharacterMovementComponent* EotRMoveComp = Cast<UEotRCharacterMovementComponent>(GetCharacterMovement()))
	{
		return EotRMoveComp->GetCurrentMovementGait();
	}
	return EEotRMovementGait::Walk;
}

FEotRLandingInfo AEotRBaseCharacter::GetLastLandingInfo() const
{
	if (const UEotRCharacterMovementComponent* EotRMoveComp = Cast<UEotRCharacterMovementComponent>(GetCharacterMovement()))
	{
		return EotRMoveComp->GetCurrentLandingInfo();
	}
	return FEotRLandingInfo();
}

void AEotRBaseCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	if (!CharacterData)
	{
		return;
	}

	USkeletalMeshComponent* FullMesh = GetMesh();

	if (CharacterData->SkeletalMeshAsset)
	{
		FullMesh->SetSkeletalMesh(CharacterData->SkeletalMeshAsset);
	}

	if (CharacterData->AnimInstanceClass)
	{
		FullMesh->SetAnimInstanceClass(CharacterData->AnimInstanceClass);
	}

	if (UEotRCharacterMovementComponent* CharacterMovementComponent = Cast<UEotRCharacterMovementComponent>(GetCharacterMovement()))
	{
		CharacterMovementComponent->SetGaitSpeedMap(CharacterData->GaitSpeedMap);
	}

	for (const TSubclassOf<UGameplayAbility>& AbilityClass : CharacterData->GameplayAbilities)
	{
		if (!AbilityClass)
		{
			continue;
		}
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(AbilityClass));
	}
}