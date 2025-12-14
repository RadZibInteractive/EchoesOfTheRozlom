// Copyright Epic Games, Inc. All Rights Reserved.

#include "EotRBaseCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Weapons/EotRWeapon.h"
#include "EotRGameplayTags.h"
#include "EotRLogChannels.h"

//////////////////////////////////////////////////////////////////////////
// AEchoesOfTheRozlomCharacter

AEotRBaseCharacter::AEotRBaseCharacter()
{
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(GetMesh());

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->AirControl = 0.5f;
}


void AEotRBaseCharacter::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void AEotRBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AEotRBaseCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AEotRBaseCharacter::Reset()
{
	K2_OnReset();
}

void AEotRBaseCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AEotRBaseCharacter::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);
}

float AEotRBaseCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	//TODO: HP from ASC

	return Damage;
}

void AEotRBaseCharacter::DoJumpStart()
{
	// pass Jump to the character
	Jump();
}

void AEotRBaseCharacter::DoJumpEnd()
{
	// pass StopJumping to the character
	StopJumping();
}

void AEotRBaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

void AEotRBaseCharacter::UnPossessed()
{
	Super::UnPossessed();
}

void AEotRBaseCharacter::OnDeathStarted(AActor*)
{
	DisableMovementAndCollision();
}

void AEotRBaseCharacter::OnDeathFinished(AActor*)
{
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::DestroyDueToDeath);
}

void AEotRBaseCharacter::DisableMovementAndCollision()
{
	if (Controller)
	{
		Controller->SetIgnoreMoveInput(true);
	}

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);

	UCharacterMovementComponent* MoveComp = CastChecked<UCharacterMovementComponent>(GetCharacterMovement());
	MoveComp->StopMovementImmediately();
	MoveComp->DisableMovement();
}

void AEotRBaseCharacter::DestroyDueToDeath()
{
	K2_OnDeathFinished();

	UninitAndDestroy();
}


void AEotRBaseCharacter::UninitAndDestroy()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		DetachFromControllerPendingDestroy();
		SetLifeSpan(0.1f);
	}

	SetActorHiddenInGame(true);
}

void AEotRBaseCharacter::DoStartFiring()
{
	// fire the current weapon
	if (CurrentWeapon)
	{
		CurrentWeapon->StartFiring();
	}
}

void AEotRBaseCharacter::DoStopFiring()
{
	// stop firing the current weapon
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFiring();
	}
}

void AEotRBaseCharacter::DoSwitchWeapon()
{
	// ensure we have at least two weapons two switch between
	if (OwnedWeapons.Num() > 1)
	{
		// deactivate the old weapon
		CurrentWeapon->DeactivateWeapon();

		// find the index of the current weapon in the owned list
		int32 WeaponIndex = OwnedWeapons.Find(CurrentWeapon);

		// is this the last weapon?
		if (WeaponIndex == OwnedWeapons.Num() - 1)
		{
			// loop back to the beginning of the array
			WeaponIndex = 0;
		}
		else {
			// select the next weapon index
			++WeaponIndex;
		}

		// set the new weapon as current
		CurrentWeapon = OwnedWeapons[WeaponIndex];

		// activate the new weapon
		CurrentWeapon->ActivateWeapon();
	}
}

void AEotRBaseCharacter::AttachWeaponMeshes(AEotRWeapon* Weapon)
{

}

void AEotRBaseCharacter::PlayFiringMontage(UAnimMontage* Montage)
{

}

void AEotRBaseCharacter::AddWeaponRecoil(float Recoil)
{

}

FVector AEotRBaseCharacter::GetWeaponTargetLocation()
{
	return FVector::ZeroVector;
}

void AEotRBaseCharacter::AddWeaponClass(const TSubclassOf<AEotRWeapon>& WeaponClass)
{
	// do we already own this weapon?
	AEotRWeapon* OwnedWeapon = FindWeaponOfType(WeaponClass);

	if (!OwnedWeapon)
	{
		// spawn the new weapon
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.TransformScaleMethod = ESpawnActorScaleMethod::MultiplyWithRoot;

		AEotRWeapon* AddedWeapon = GetWorld()->SpawnActor<AEotRWeapon>(WeaponClass, GetActorTransform(), SpawnParams);

		if (AddedWeapon)
		{
			// add the weapon to the owned list
			OwnedWeapons.Add(AddedWeapon);

			// if we have an existing weapon, deactivate it
			if (CurrentWeapon)
			{
				CurrentWeapon->DeactivateWeapon();
			}

			// switch to the new weapon
			CurrentWeapon = AddedWeapon;
			CurrentWeapon->ActivateWeapon();
		}
	}
}

void AEotRBaseCharacter::OnWeaponActivated(AEotRWeapon* Weapon)
{

}

void AEotRBaseCharacter::OnWeaponDeactivated(AEotRWeapon* Weapon)
{
	// unused
}

void AEotRBaseCharacter::OnSemiWeaponRefire()
{
	// unused
}

AEotRWeapon* AEotRBaseCharacter::FindWeaponOfType(TSubclassOf<AEotRWeapon> WeaponClass) const
{
	// check each owned weapon
	for (AEotRWeapon* Weapon : OwnedWeapons)
	{
		if (Weapon->IsA(WeaponClass))
		{
			return Weapon;
		}
	}

	// weapon not found
	return nullptr;

}