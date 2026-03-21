// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "AbilitySystemInterface.h"
#include "Engine/DataAsset.h"

#include "Interfaces/EotRTeamAgentInterface.h"
#include "Interfaces/EotRInteractionInterface.h"
#include "Interfaces/CombatInterface.h"
#include "Interfaces/SaveInterface.h"

#include "Data/Structs/EotRLandingInfo.h"
#include "Data/DataAssets/Characters/EotRCharacterDataAsset.h"

class UEotRAbilitySystemComponent;
class UAbilitySystemComponent;
class UCombatComponent;
class UEotRTeamAgentComponent;

#include "EotRBaseCharacter.generated.h"

UCLASS()
class ECHOESOFTHEROZLOM_API AEotRBaseCharacter
	: public ACharacter
	, public IAbilitySystemInterface
	, public IEotRTeamAgentInterface
	, public IEotRInteractionInterface
	, public ICombatInterface
	, public ISaveInterface
{
	GENERATED_BODY()

public:
	AEotRBaseCharacter(const FObjectInitializer& ObjectInitializer);

	// Mesh
	virtual void SetAbilityAnimTarget(bool bUseAlternative) {}

	// AnimBP
	UFUNCTION(BlueprintPure, Category = "EotR|Movement")
	EEotRMovementGait GetLastMovementGait() const;

	UFUNCTION(BlueprintPure, Category = "EotR|Movement")
	FEotRLandingInfo GetLastLandingInfo() const;

	// IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default")
	TObjectPtr<UEotRAbilitySystemComponent> AbilitySystemComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default")
	TObjectPtr<UEotRTeamAgentComponent> TeamAgentComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default")
	TObjectPtr<UCombatComponent> CombatComponent = nullptr;
};
