// Copyright Epic Games, Inc. All Rights Reserved.


#include "Player/EotRPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Characters/EotRPlayerCharacter.h"

void AEotRPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void AEotRPlayerController::SetupInputComponent()
{
	// add the input mapping contexts
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
		{
			Subsystem->AddMappingContext(CurrentContext, 0);
		}
	}
}

void AEotRPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// subscribe to the pawn's OnDestroyed delegate
	InPawn->OnDestroyed.AddUniqueDynamic(this, &AEotRPlayerController::OnPawnDestroyed);
}

void AEotRPlayerController::OnPawnDestroyed(AActor* DestroyedActor)
{
}
