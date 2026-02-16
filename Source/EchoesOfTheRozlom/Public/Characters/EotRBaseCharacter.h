#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "AbilitySystemInterface.h"
#include "Engine/DataAsset.h"

#include "Interfaces/EotRTeamAgentInterface.h"
#include "Interfaces/InteractableInterface.h"
#include "Interfaces/InteractionInterface.h"
#include "Interfaces/CombatInterface.h"
#include "Interfaces/SaveInterface.h"

#include "Data/Enums/EotRMovementGait.h"
#include "Data/Structs/EotRLandingInfo.h"

class UAbilitySystemComponent;
class UInteractableComponent;
class UInteractionComponent;
class UCombatComponent;
class UEotRTeamAgentComponent;
class UGameplayAbility;

#include "EotRBaseCharacter.generated.h"

UCLASS()
class ECHOESOFTHEROZLOM_API UEotRCharacterDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UEotRCharacterDataAsset()
	{
		GaitSpeedMap.Add(EEotRMovementGait::Walk, FVector(200.f, 180.f, 150.f));
	}
	UPROPERTY(EditAnywhere, Category = "Mesh")
	TObjectPtr<USkeletalMesh> SkeletalMeshAsset = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mesh")
	TSubclassOf<UAnimInstance> AnimInstanceClass = nullptr;

	UPROPERTY(EditAnywhere, Category = "Movement", meta = (Tooltip = "Movement mode speeds: X = forward, Y = sideways, Z = backward"))
	TMap<EEotRMovementGait, FVector> GaitSpeedMap;

	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> GameplayAbilities;
};

UCLASS()
class ECHOESOFTHEROZLOM_API AEotRBaseCharacter
	: public ACharacter
	, public IAbilitySystemInterface
	, public IEotRTeamAgentInterface
	, public IInteractableInterface
	, public IInteractionInterface
	, public ICombatInterface
	, public ISaveInterface
{
	GENERATED_BODY()

public:
	AEotRBaseCharacter(const FObjectInitializer& ObjectInitializer);

	// Mesh
	virtual void SetAbilityAnimTarget(bool bUseAlternative) {}

	// AnimBP
	UFUNCTION(BlueprintPure, Category = "EotR|Animation")
	EEotRMovementGait GetLastMovementGait() const;

	UFUNCTION(BlueprintPure, Category = "EotR|Animation")
	FEotRLandingInfo GetLastLandingInfo() const;

	// IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }

protected:
	// AActor
	virtual void PostInitializeComponents() override;

	// APawn
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

protected:
	// Data
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Default")
	TObjectPtr<UEotRCharacterDataAsset> CharacterData = nullptr;
	
	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default")
	TObjectPtr<UEotRTeamAgentComponent> EotRTeamAgentComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default")
	TObjectPtr<UInteractableComponent> InteractableComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default")
	TObjectPtr<UInteractionComponent> InteractionComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default")
	TObjectPtr<UCombatComponent> CombatComponent = nullptr;
};
