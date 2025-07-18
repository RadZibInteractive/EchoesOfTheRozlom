// Copyright Epic Games, Inc. All Rights Reserved.

#include "EotRCombatSet.h"

#include "AbilitySystem/Attributes/EotRAttributeSet.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EotRCombatSet)

class FLifetimeProperty;


UEotRCombatSet::UEotRCombatSet()
	: BaseDamage(0.0f)
	, BaseHeal(0.0f)
{
}

void UEotRCombatSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UEotRCombatSet, BaseDamage, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UEotRCombatSet, BaseHeal, COND_OwnerOnly, REPNOTIFY_Always);
}

void UEotRCombatSet::OnRep_BaseDamage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UEotRCombatSet, BaseDamage, OldValue);
}

void UEotRCombatSet::OnRep_BaseHeal(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UEotRCombatSet, BaseHeal, OldValue);
}

