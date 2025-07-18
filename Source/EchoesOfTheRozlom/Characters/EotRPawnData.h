// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"

#include "EotRPawnData.generated.h"

class APawn;
class UEotRAbilitySet;
class UEotRAbilityTagRelationshipMapping;
class UEotRInputConfig;
class UObject;


/**
 * UEotRPawnData
 *
 *	Non-mutable data asset that contains properties used to define a pawn.
 */
UCLASS(BlueprintType, Const, Meta = (DisplayName = "EotR Pawn Data", ShortTooltip = "Data asset used to define a Pawn."))
class ECHOESOFTHEROZLOM_API UEotRPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UEotRPawnData(const FObjectInitializer& ObjectInitializer);

public:

	// Class to instantiate for this pawn (should usually derive from AEotRPawn or AEotRCharacter).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EotR|Pawn")
	TSubclassOf<APawn> PawnClass;

	// Ability sets to grant to this pawn's ability system.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EotR|Abilities")
	TArray<TObjectPtr<UEotRAbilitySet>> AbilitySets;

	// What mapping of ability tags to use for actions taking by this pawn
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EotR|Abilities")
	TObjectPtr<UEotRAbilityTagRelationshipMapping> TagRelationshipMapping;
};
