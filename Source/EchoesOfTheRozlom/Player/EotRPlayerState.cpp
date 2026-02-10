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

// Required by IAbilitySystemInterface
UAbilitySystemComponent* AEotRPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AEotRPlayerState::SetPawnData(const UEotRPawnData* InPawnData)
{
	if (PawnData == InPawnData)
	{
		return;
	}

	PawnData = InPawnData;

	// If we are authority, call the handler immediately as well.
	// Clients will get OnRep_PawnData automatically.
	OnRep_PawnData();
}

void AEotRPlayerState::OnRep_PawnData()
{
	// Minimal safe handler.
	// If your game uses PawnData to grant abilities/effects, that logic likely lives elsewhere.
}

const FName AEotRPlayerState::NAME_EotRAbilityReady(TEXT("EotR.AbilityReady"));

void AEotRPlayerState::SetPlayerConnectionType(EEotRPlayerConnectionType NewType)
{
	MyPlayerConnectionType = NewType;
}

void AEotRPlayerState::AddStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	StatTags.AddStack(Tag, StackCount);
}

void AEotRPlayerState::RemoveStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	StatTags.RemoveStack(Tag, StackCount);
}

int32 AEotRPlayerState::GetStatTagStackCount(FGameplayTag Tag) const
{
	return StatTags.GetStackCount(Tag);
}

bool AEotRPlayerState::HasStatTag(FGameplayTag Tag) const
{
	return StatTags.HasTag(Tag);
}

// NOTE: The original project likely has a real message router implementation.
// For now this is a safe no-op to keep compilation/linking intact.
void AEotRPlayerState::ClientBroadcastMessage_Implementation(const FEotRVerbMessage Message)
{
	// Intentionally empty
}

void AEotRPlayerState::Reset()
{
	Super::Reset();
}

void AEotRPlayerState::ClientInitialize(AController* C)
{
	Super::ClientInitialize(C);
}

void AEotRPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);
}

void AEotRPlayerState::OnDeactivated()
{
	Super::OnDeactivated();
}

void AEotRPlayerState::OnReactivated()
{
	Super::OnReactivated();
}

void AEotRPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEotRPlayerState, PawnData);
	DOREPLIFETIME(AEotRPlayerState, MyPlayerConnectionType);
	DOREPLIFETIME(AEotRPlayerState, StatTags);
}