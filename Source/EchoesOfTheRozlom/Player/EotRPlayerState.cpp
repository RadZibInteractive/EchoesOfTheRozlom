// Copyright Epic Games, Inc. All Rights Reserved.

#include "EotRPlayerState.h"

#include "Engine/World.h"
#include "FrameworkBase/EotRBaseGameMode.h"
#include "EotRLogChannels.h"
#include "EotRPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystem/Attributes/EotRResonanceSet.h"

class AController;
class APlayerState;
class FLifetimeProperty;

AEotRPlayerState::AEotRPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Create and configure the Ability System Component (ASC)
	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UEotRAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// Attribute sets (kept as UPROPERTY refs so they don’t get GC’d before ASC init)
	HealthSet = CreateDefaultSubobject<UEotRHealthSet>(TEXT("HealthSet"));
	CombatSet = CreateDefaultSubobject<UEotRCombatSet>(TEXT("CombatSet"));

	// Resonance attribute set (Stress + ResonanceEnergy)
	ResonanceSet = CreateDefaultSubobject<UEotRResonanceSet>(TEXT("ResonanceSet"));

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
    
    // Initialize GAS actor info: Owner = PlayerState, Avatar = Pawn
	check(AbilitySystemComponent);
	AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());
}
