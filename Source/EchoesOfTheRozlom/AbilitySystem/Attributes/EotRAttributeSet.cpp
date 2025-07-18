// Copyright Epic Games, Inc. All Rights Reserved.

#include "EotRAttributeSet.h"

#include "AbilitySystem/EotRAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EotRAttributeSet)

class UWorld;


UEotRAttributeSet::UEotRAttributeSet()
{
}

UWorld* UEotRAttributeSet::GetWorld() const
{
	const UObject* Outer = GetOuter();
	check(Outer);

	return Outer->GetWorld();
}

UEotRAbilitySystemComponent* UEotRAttributeSet::GetEotRAbilitySystemComponent() const
{
	return Cast<UEotRAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}

