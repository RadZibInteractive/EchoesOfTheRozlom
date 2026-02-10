// © 2025 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "EotRGameplayAbility.generated.h"

/**
 * Base GameplayAbility class for the project.
 *
 * AbilitySets (UEotRAbilitySet) are typed to TSubclassOf<UEotRGameplayAbility>,
 * so this class must exist for abilities like GA_Resonance_Identify to be assignable.
 */
UCLASS(Abstract, Blueprintable)
class ECHOESOFTHEROZLOM_API UEotRGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UEotRGameplayAbility();
};
