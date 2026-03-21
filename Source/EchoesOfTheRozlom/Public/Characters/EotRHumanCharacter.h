// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Characters/EotRBaseCharacter.h"

#include "Interfaces/InventoryInterface.h"
#include "Interfaces/EquipmentInterface.h"
#include "Interfaces/WeaponInterface.h"
#include "Interfaces/DialogueInterface.h"

#include "GameplayTagContainer.h"

#include "Data/DataAssets/Characters/EotRHumanDataAsset.h"

class USkeletalMeshComponent;
class UEotRCameraComponent;
struct FInputActionValue;

class UEotRInteractionComponent;
class UInventoryComponent;
class UEquipmentComponent;
class UWeaponComponent;
class UDialogueComponent;

#include "EotRHumanCharacter.generated.h"

UCLASS()
class ECHOESOFTHEROZLOM_API AEotRHumanCharacter
	: public AEotRBaseCharacter
	, public IInventoryInterface
	, public IEquipmentInterface
	, public IWeaponInterface
	, public IDialogueInterface
{
	GENERATED_BODY()

public:
	AEotRHumanCharacter(const FObjectInitializer& ObjectInitializer);

	// Mesh
	virtual void SetAbilityAnimTarget(bool bUseAlternative) override;

	// Camera
	UEotRCameraComponent* GetFirstPersonCamera() { return FirstPersonCamera; }

protected:
	// AActor
	virtual void PostInitializeComponents() override;

	// APawn
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	// Input
	void OnInputTagTriggered(const FInputActionValue& ActionValue, FGameplayTag InputTag);
	float CalculateMoveInput(bool bIsRightAxis, float ActionValue, ECollisionChannel TraceChannel = ECC_GameTraceChannel1) const;
	float CalculateCameraInput(bool bIsYawAxis, float ActionValue) const;

protected:
	// Mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default")
	TObjectPtr<USkeletalMeshComponent> UpperMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default")
	TArray<TObjectPtr<USkeletalMeshComponent>> ViewPartMeshes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default")
	TObjectPtr<USkeletalMeshComponent> ViewHeadMesh = nullptr;

	// Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default")
	TObjectPtr<UEotRCameraComponent> FirstPersonCamera = nullptr;

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default")
	TObjectPtr<UEotRInteractionComponent> InteractionComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default")
	TObjectPtr<UInventoryComponent> InventoryComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default")
	TObjectPtr<UEquipmentComponent> EquipmentComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default")
	TObjectPtr<UWeaponComponent> WeaponComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default")
	TObjectPtr<UDialogueComponent> DialogueComponent = nullptr;

private:
	FName FirstPersonCameraSocket;
};
