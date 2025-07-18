// Copyright Epic Games, Inc. All Rights Reserved.

#include "EotRPlayerState.h"

#include "AbilitySystem/Attributes/EotRCombatSet.h"
#include "AbilitySystem/Attributes/EotRHealthSet.h"
#include "AbilitySystem/EotRAbilitySet.h"
#include "AbilitySystem/EotRAbilitySystemComponent.h"
#include "Characters/EotRPawnData.h"
#include "Characters/Components/EotRPawnExtensionComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Engine/World.h"
#include "GameFramework/GameplayMessageSubsystem.h"
//@TODO: Would like to isolate this a bit better to get the pawn data in here without this having to know about other stuff
#include "GameModes/EotRBaseGameMode.h"
#include "EotRLogChannels.h"
#include "EotRPlayerController.h"
#include "Messages/EotRVerbMessage.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EotRPlayerState)

class AController;
class APlayerState;
class FLifetimeProperty;

const FName AEotRPlayerState::NAME_EotRAbilityReady("EotRAbilitiesReady");

AEotRPlayerState::AEotRPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, MyPlayerConnectionType(EEotRPlayerConnectionType::Player)
{
	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UEotRAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// These attribute sets will be detected by AbilitySystemComponent::InitializeComponent. Keeping a reference so that the sets don't get garbage collected before that.
	HealthSet = CreateDefaultSubobject<UEotRHealthSet>(TEXT("HealthSet"));
	CombatSet = CreateDefaultSubobject<UEotRCombatSet>(TEXT("CombatSet"));

	// AbilitySystemComponent needs to be updated at a high frequency.
	SetNetUpdateFrequency(100.0f);
}

void AEotRPlayerState::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void AEotRPlayerState::Reset()
{
	Super::Reset();
}

void AEotRPlayerState::ClientInitialize(AController* C)
{
	Super::ClientInitialize(C);

	if (UEotRPawnExtensionComponent* PawnExtComp = UEotRPawnExtensionComponent::FindPawnExtensionComponent(GetPawn()))
	{
		PawnExtComp->CheckDefaultInitialization();
	}
}

void AEotRPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	//@TODO: Copy stats
}

void AEotRPlayerState::OnDeactivated()
{
	bool bDestroyDeactivatedPlayerState = false;

	switch (GetPlayerConnectionType())
	{
		case EEotRPlayerConnectionType::Player:
		case EEotRPlayerConnectionType::InactivePlayer:
			//@TODO: Ask the experience if we should destroy disconnecting players immediately or leave them around
			// (e.g., for long running servers where they might build up if lots of players cycle through)
			bDestroyDeactivatedPlayerState = true;
			break;
		default:
			bDestroyDeactivatedPlayerState = true;
			break;
	}
	
	SetPlayerConnectionType(EEotRPlayerConnectionType::InactivePlayer);

	if (bDestroyDeactivatedPlayerState)
	{
		Destroy();
	}
}

void AEotRPlayerState::OnReactivated()
{
	if (GetPlayerConnectionType() == EEotRPlayerConnectionType::InactivePlayer)
	{
		SetPlayerConnectionType(EEotRPlayerConnectionType::Player);
	}
}

void AEotRPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, PawnData, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, MyPlayerConnectionType, SharedParams)
	DOREPLIFETIME(ThisClass, StatTags);	
}

AEotRPlayerController* AEotRPlayerState::GetEotRPlayerController() const
{
	return Cast<AEotRPlayerController>(GetOwner());
}

UAbilitySystemComponent* AEotRPlayerState::GetAbilitySystemComponent() const
{
	return GetEotRAbilitySystemComponent();
}

void AEotRPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(AbilitySystemComponent);
	AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());
}

void AEotRPlayerState::SetPawnData(const UEotRPawnData* InPawnData)
{
	check(InPawnData);

	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (PawnData)
	{
		UE_LOG(LogEotR, Error, TEXT("Trying to set PawnData [%s] on player state [%s] that already has valid PawnData [%s]."), *GetNameSafe(InPawnData), *GetNameSafe(this), *GetNameSafe(PawnData));
		return;
	}

	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, PawnData, this);
	PawnData = InPawnData;

	for (const UEotRAbilitySet* AbilitySet : PawnData->AbilitySets)
	{
		if (AbilitySet)
		{
			AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);
		}
	}

	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, NAME_EotRAbilityReady);
	
	ForceNetUpdate();
}

void AEotRPlayerState::OnRep_PawnData()
{
}

void AEotRPlayerState::SetPlayerConnectionType(EEotRPlayerConnectionType NewType)
{
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, MyPlayerConnectionType, this);
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
	return StatTags.ContainsTag(Tag);
}

void AEotRPlayerState::ClientBroadcastMessage_Implementation(const FEotRVerbMessage Message)
{
	// This check is needed to prevent running the action when in standalone mode
	if (GetNetMode() == NM_Client)
	{
		UGameplayMessageSubsystem::Get(this).BroadcastMessage(Message.Verb, Message);
	}
}

