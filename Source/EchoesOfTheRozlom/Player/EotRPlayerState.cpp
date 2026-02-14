// Copyright Epic Games, Inc. All Rights Reserved.

#include "EotRPlayerState.h"

#include "Engine/World.h"
#include "FrameworkBase/EotRGameMode.h"
#include "EotRLogChannels.h"
#include "EotRPlayerController.h"
#include "Net/UnrealNetwork.h"

class AController;
class APlayerState;
class FLifetimeProperty;

AEotRPlayerState::AEotRPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//TODO: Add AbilitySystemComponent

	// AbilitySystemComponent needs to be updated at a high frequency.
	SetNetUpdateFrequency(100.0f);
}

void AEotRPlayerState::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

AEotRPlayerController* AEotRPlayerState::GetEotRPlayerController() const
{
	return Cast<AEotRPlayerController>(GetOwner());
}


void AEotRPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	//TODO: Init ASC
}
