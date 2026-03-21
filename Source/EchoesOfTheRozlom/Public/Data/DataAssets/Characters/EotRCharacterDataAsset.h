// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ForgeDataAsset.h"
#include "Data/Enums/EotRMovementGait.h"
#include "EotRCharacterDataAsset.generated.h"

class UGameplayAbility;

UCLASS()
class ECHOESOFTHEROZLOM_API UEotRCharacterDataAsset : public UForgeDataAsset
{
	GENERATED_BODY()

public:
	UEotRCharacterDataAsset()
	{
		GaitSpeedMap.Add(EEotRMovementGait::Walk, FVector(200.f, 180.f, 150.f));
	}
	UPROPERTY(EditAnywhere, Category = "Mesh", meta = (Tooltip = "Main character mesh, either used or referenced for movement"))
	TObjectPtr<USkeletalMesh> SkeletalMeshAsset = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mesh", meta = (Tooltip = "Anim BP for main character mesh"))
	TSubclassOf<UAnimInstance> AnimInstanceClass = nullptr;

	UPROPERTY(EditAnywhere, Category = "Movement", meta = (Tooltip = "Movement mode speeds: X = forward, Y = sideways, Z = backward"))
	TMap<EEotRMovementGait, FVector> GaitSpeedMap;

	UPROPERTY(EditAnywhere, Category = "Abilities", meta = (Tooltip = "Array of all character granted abilities"))
	TArray<TSubclassOf<UGameplayAbility>> GameplayAbilities;
};