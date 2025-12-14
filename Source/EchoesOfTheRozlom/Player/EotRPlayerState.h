// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"

#include "EotRPlayerState.generated.h"

class AEotRPlayerController;

/**
 * AEotRPlayerState
 *
 *	Base player state class used by this project.
 */
UCLASS(Config = Game)
class ECHOESOFTHEROZLOM_API AEotRPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AEotRPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "EotR|PlayerState")
	AEotRPlayerController* GetEotRPlayerController() const;

	//~AActor interface
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	//~End of AActor interface
};
