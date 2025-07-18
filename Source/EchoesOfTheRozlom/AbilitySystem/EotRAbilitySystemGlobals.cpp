// Copyright Epic Games, Inc. All Rights Reserved.

#include "EotRAbilitySystemGlobals.h"

#include "EotRGameplayEffectContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EotRAbilitySystemGlobals)

struct FGameplayEffectContext;

UEotRAbilitySystemGlobals::UEotRAbilitySystemGlobals(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FGameplayEffectContext* UEotRAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FEotRGameplayEffectContext();
}

