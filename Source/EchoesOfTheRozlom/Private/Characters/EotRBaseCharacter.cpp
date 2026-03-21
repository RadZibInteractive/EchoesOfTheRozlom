#include "Characters/EotRBaseCharacter.h"

#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"

#include "Components/EotRCharacterMovementComponent.h"
#include "AbilitySystem/EotRAbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"

AEotRBaseCharacter::AEotRBaseCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UEotRCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	UCapsuleComponent* Capsule = GetCapsuleComponent();
	Capsule->SetCapsuleHalfHeight(86.f);
	Capsule->SetCapsuleRadius(30.f);
	Capsule->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Ignore);
		
	USkeletalMeshComponent* BodyMesh = GetMesh();
	BodyMesh->SetRelativeLocation(FVector(0.f, 0.f, -Capsule->GetUnscaledCapsuleHalfHeight() - 2.f));
	BodyMesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	AbilitySystemComponent = CreateDefaultSubobject<UEotRAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
}

void AEotRBaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AbilitySystemComponent->InitAbilityActorInfo(NewController, this);
}

void AEotRBaseCharacter::UnPossessed()
{
	Super::UnPossessed();

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
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

UAbilitySystemComponent* AEotRBaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}

void AEotRBaseCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	if (!CharacterData)
	{
		return;
	}

	USkeletalMeshComponent* BodyMesh = GetMesh();

	if (CharacterData->SkeletalMeshAsset)
	{
		BodyMesh->SetSkeletalMesh(CharacterData->SkeletalMeshAsset);
	}

	if (CharacterData->AnimInstanceClass)
	{
		BodyMesh->SetAnimInstanceClass(CharacterData->AnimInstanceClass);
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