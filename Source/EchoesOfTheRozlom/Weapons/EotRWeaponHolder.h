// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EotRWeaponHolder.generated.h"

class AEotRWeapon;
class UAnimMontage;


// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEotRWeaponHolder : public UInterface
{
	GENERATED_BODY()
};

/**
 *  Common interface for EotR Game weapon holder classes
 */
class ECHOESOFTHEROZLOM_API IEotRWeaponHolder
{
	GENERATED_BODY()

public:

	/** Attaches a weapon's meshes to the owner */
	virtual void AttachWeaponMeshes(AEotRWeapon* Weapon) = 0;

	/** Plays the firing montage for the weapon */
	virtual void PlayFiringMontage(UAnimMontage* Montage) = 0;

	/** Applies weapon recoil to the owner */
	virtual void AddWeaponRecoil(float Recoil) = 0;

	/** Calculates and returns the aim location for the weapon */
	virtual FVector GetWeaponTargetLocation() = 0;

	/** Gives a weapon of this class to the owner */
	virtual void AddWeaponClass(const TSubclassOf<AEotRWeapon>& WeaponClass) = 0;

	/** Activates the passed weapon */
	virtual void OnWeaponActivated(AEotRWeapon* Weapon) = 0;

	/** Deactivates the passed weapon */
	virtual void OnWeaponDeactivated(AEotRWeapon* Weapon) = 0;

	/** Notifies the owner that the weapon cooldown has expired and it's ready to shoot again */
	virtual void OnSemiWeaponRefire() = 0;
};
