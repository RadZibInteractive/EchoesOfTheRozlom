// Copyright Epic Games, Inc. All Rights Reserved.


#include "AI/EotRNPC.h"
#include "Weapons/EotRWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "TimerManager.h"

AEotRNPC::AEotRNPC()
{
}

void AEotRNPC::BeginPlay()
{
	Super::BeginPlay();
}

void AEotRNPC::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

float AEotRNPC::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	return Damage;
}

void AEotRNPC::AttachWeaponMeshes(AEotRWeapon* WeaponToAttach)
{
	Super::AttachWeaponMeshes(WeaponToAttach);

	const FAttachmentTransformRules AttachmentRule(EAttachmentRule::SnapToTarget, false);

	// attach the weapon actor
	WeaponToAttach->AttachToActor(this, AttachmentRule);
	WeaponToAttach->GetThirdPersonMesh()->AttachToComponent(GetSkeletalMesh(), AttachmentRule, ThirdPersonWeaponSocket);
}

void AEotRNPC::PlayFiringMontage(UAnimMontage* Montage)
{
	Super::PlayFiringMontage(Montage);
}

void AEotRNPC::AddWeaponRecoil(float Recoil)
{
	Super::AddWeaponRecoil(Recoil);
}

FVector AEotRNPC::GetWeaponTargetLocation()
{
	return Super::GetWeaponTargetLocation();
}

void AEotRNPC::AddWeaponClass(const TSubclassOf<AEotRWeapon>& WeaponClass)
{
	Super::AddWeaponClass(WeaponClass);
}

void AEotRNPC::OnWeaponActivated(AEotRWeapon* Weapon)
{
	Super::OnWeaponActivated(Weapon);
}

void AEotRNPC::OnWeaponDeactivated(AEotRWeapon* Weapon)
{
	Super::OnWeaponDeactivated(Weapon);
}

void AEotRNPC::OnSemiWeaponRefire()
{
	Super::OnSemiWeaponRefire();
}
