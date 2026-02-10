#pragma once

#include "Engine/DataAsset.h"
#include "AbilitySystem/EotRAbilitySet.h"
#include "EotRPawnData.generated.h"

class UEotRAbilitySet;

/**
 * PawnData used by PlayerState to grant ability sets.
 * Minimal stub: keeps AbilitySets array because your PlayerState references it.
 */
UCLASS(BlueprintType, Const)
class ECHOESOFTHEROZLOM_API UEotRPawnData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
	TArray<TObjectPtr<const UEotRAbilitySet>> AbilitySets;
};
