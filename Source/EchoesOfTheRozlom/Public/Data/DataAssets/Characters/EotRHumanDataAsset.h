// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/DataAssets/Characters/EotRCharacterDataAsset.h"
#include "Data/Enums/EotRMovementGait.h"
#include "AbilitySystem/Abilities/EotRGA_Run.h"
#include "EotRHumanDataAsset.generated.h"

UCLASS()
class ECHOESOFTHEROZLOM_API UEotRHumanDataAsset : public UEotRCharacterDataAsset
{
	GENERATED_BODY()

public:
	UEotRHumanDataAsset()
	{
		GaitSpeedMap.Add(EEotRMovementGait::Run, FVector(500.f, 350.f, 300.f));
		GameplayAbilities.Add(UEotRGA_Run::StaticClass());
	}

	UPROPERTY(EditAnywhere, Category = "Mesh", meta = (Tooltip = "Implied that main character mesh referenced only"))
	TObjectPtr<USkeletalMesh> UpperMeshAsset = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mesh", meta = (Tooltip = "Implied that upper mesh comes without head"))
	TObjectPtr<USkeletalMesh> HeadMeshAsset = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mesh", meta = (Tooltip = "For extra modular parts if needed"))
	TArray<TObjectPtr<USkeletalMesh>> PartMeshesAssets;

	UPROPERTY(EditAnywhere, Category = "Mesh", meta = (Tooltip = "Animation Blueprint that references and extends main one"))
	TSubclassOf<UAnimInstance> UpperAnimInstanceClass = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mesh")
	FName FirstPersonCameraSocketName = TEXT("FPCamera");

	UPROPERTY(EditAnywhere, Category = "Mesh")
	FName WeaponSocketName = TEXT("VB ik_hand_gun");
};