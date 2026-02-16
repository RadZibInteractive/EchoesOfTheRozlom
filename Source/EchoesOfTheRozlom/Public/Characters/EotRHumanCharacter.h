#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Characters/EotRBaseCharacter.h"

#include "Interfaces/InventoryInterface.h"
#include "Interfaces/EquipmentInterface.h"
#include "Interfaces/WeaponInterface.h"
#include "Interfaces/DialogueInterface.h"

#include "GameplayTagContainer.h"

#include "Data/Enums/EotRMovementGait.h"
#include "AbilitySystem/Abilities/EotRGA_Run.h"

class USkeletalMeshComponent;
class UEotRCameraComponent;
struct FInputActionValue;

class UInventoryComponent;
class UEquipmentComponent;
class UWeaponComponent;
class UDialogueComponent;

#include "EotRHumanCharacter.generated.h"

UCLASS()
class ECHOESOFTHEROZLOM_API UEotRHumanDataAsset : public UEotRCharacterDataAsset
{
	GENERATED_BODY()

public:
	UEotRHumanDataAsset()
	{
		GaitSpeedMap.Add(EEotRMovementGait::Run, FVector(500.f, 350.f, 300.f));
		GameplayAbilities.Add(UEotRGA_Run::StaticClass());
	}

	UPROPERTY(EditAnywhere, Category = "Mesh", meta = (Tooltip = "Implied that main character mesh referenced only"))
	TObjectPtr<USkeletalMesh> UpperMeshAsset = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mesh", meta = (Tooltip = "Implied that upper mesh comes without head"))
	TObjectPtr<USkeletalMesh> HeadMeshAsset = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mesh", meta = (Tooltip = "For extra modular parts if needed"))
	TArray<TObjectPtr<USkeletalMesh>> PartMeshesAssets;

	UPROPERTY(EditAnywhere, Category = "Mesh", meta = (Tooltip = "Animation Blueprint that references and extends main one"))
	TSubclassOf<UAnimInstance> UpperAnimInstanceClass = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mesh")
	FName FirstPersonCameraSocketName = TEXT("FPCamera");

	UPROPERTY(EditAnywhere, Category = "Mesh")
	FName WeaponSocketName = TEXT("VB ik_hand_gun");
};

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
	USkeletalMeshComponent* GetUpperMesh() { return UpperMesh; }

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
	float CalculateMoveInput(bool bIsRightAxis, float ActionValue, ECollisionChannel TraceChannel = ECC_Visibility) const;
	float CalculateCameraInput(bool bIsYawAxis, float ActionValue) const;

protected:
	// Mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default")
	TObjectPtr<USkeletalMeshComponent> UpperMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default")
	TObjectPtr<USkeletalMeshComponent> HeadMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default")
	TArray<TObjectPtr<USkeletalMeshComponent>> PartMeshes;

	// Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default")
	TObjectPtr<UEotRCameraComponent> FirstPersonCamera = nullptr;

	// Components
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

	FName WeaponSocket;
};
