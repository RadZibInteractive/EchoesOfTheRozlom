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

	// is this a EotR character?
	if (AEotRPlayerCharacter* EotRPlayerCharacter = Cast<AEotRPlayerCharacter>(InPawn))
	{
		// add the player tag
		EotRPlayerCharacter->Tags.Add(PlayerPawnTag);
	}
}

void AEotRPlayerController::OnPawnDestroyed(AActor* DestroyedActor)
{
	// find the player start
	TArray<AActor*> ActorList;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), ActorList);

	if (ActorList.Num() > 0)
	{
		// select a random player start
		AActor* RandomPlayerStart = ActorList[FMath::RandRange(0, ActorList.Num() - 1)];

		// spawn a character at the player start
		const FTransform SpawnTransform = RandomPlayerStart->GetActorTransform();

		if (AEotRPlayerCharacter* RespawnedCharacter = GetWorld()->SpawnActor<AEotRPlayerCharacter>(CharacterClass, SpawnTransform))
		{
			// possess the character
			Possess(RespawnedCharacter);
		}
	}
}
