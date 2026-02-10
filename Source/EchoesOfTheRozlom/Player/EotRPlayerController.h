// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AbilitySystem/EotRAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/EotRHealthSet.h"
#include "AbilitySystem/Attributes/EotRCombatSet.h"
#include "Messages/EotRVerbMessage.h"
#include "Player/EotRPawnData.h"
#include "EotRPlayerController.generated.h"

class UInputMappingContext;
class AEotRPlayerCharacter;

/**
 *  Simple PlayerController for a first person EotR game
 *  Manages input mappings
 *  Respawns the player pawn when it's destroyed
 */
UCLASS(abstract)
class ECHOESOFTHEROZLOM_API AEotRPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:

	/** Input mapping contexts for this player */
	UPROPERTY(EditAnywhere, Category="EotR")
	TArray<UInputMappingContext*> DefaultMappingContexts;

protected:

	/** Gameplay Initialization */
	virtual void BeginPlay() override;

	/** Initialize input bindings */
	virtual void SetupInputComponent() override;

	/** Pawn initialization */
	virtual void OnPossess(APawn* InPawn) override;

	/** Called if the possessed pawn is destroyed */
	UFUNCTION()
	void OnPawnDestroyed(AActor* DestroyedActor);
};
