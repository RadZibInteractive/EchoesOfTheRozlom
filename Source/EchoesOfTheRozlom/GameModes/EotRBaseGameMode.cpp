// Copyright Epic Games, Inc. All Rights Reserved.


#include "GameModes/EotRBaseGameMode.h"
#include "Player/EotRPlayerState.h"
#include "Player/EotRPlayerController.h"
#include "Characters/EotRPlayerCharacter.h"
#include "Characters/Components/EotRPawnExtensionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

AEotRBaseGameMode::AEotRBaseGameMode(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
    PlayerControllerClass = AEotRPlayerController::StaticClass();
    PlayerStateClass = AEotRPlayerState::StaticClass();
    DefaultPawnClass = AEotRPlayerCharacter::StaticClass();
}

void AEotRBaseGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
    Super::InitGame(MapName, Options, ErrorMessage);
}

void AEotRBaseGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void AEotRBaseGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    if (AEotRPlayerState* PS = Cast<AEotRPlayerState>(NewPlayer->PlayerState))
    {
        const UEotRPawnData* PawnData = DefaultPawnData;
        if (PawnData)
        {
            PS->SetPawnData(PawnData);

            if (APawn* Pawn = NewPlayer->GetPawn())
            {
                if (UEotRPawnExtensionComponent* Ext = UEotRPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
                {
                    Ext->SetPawnData(DefaultPawnData);
                }
            }
        }
    }
}