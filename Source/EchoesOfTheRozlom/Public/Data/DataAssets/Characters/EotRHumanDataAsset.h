// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/DataAssets/Characters/EotRCharacterDataAsset.h"
#include "Data/Enums/EotRMovementGait.h"
#include "AbilitySystem/Abilities/EotRGA_Run.h"
#include "AbilitySystem/Abilities/EotRGA_Interact.h"
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
		GameplayAbilities.Add(UEotRGA_Interact::StaticClass());
	}

	UPROPERTY(EditAnywhere, Category = "Mesh", meta = (Tooltip = "Hidden mesh for upper body, to reference weapon/combat animations"))
	TObjectPtr<USkeletalMesh> UpperMeshAsset = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mesh", meta = (Tooltip = "Anim BP for upper body"))
	TSubclassOf<UAnimInstance> UpperAnimInstanceClass = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mesh", meta = (Tooltip = "Finally, all parts of character that will be actually displayed (without head)"))
	TArray<TObjectPtr<USkeletalMesh>> ViewPartMeshesAssets;

	UPROPERTY(EditAnywhere, Category = "Mesh", meta = (Tooltip = "Head mesh, hidden for owner but casts shadows and visible outside"))
	TObjectPtr<USkeletalMesh> ViewHeadMeshAsset = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mesh", meta = (Tooltip = "Anim BP for display mesh"))
	TSubclassOf<UAnimInstance> ViewAnimInstanceClass = nullptr;

	UPROPERTY(EditAnywhere, Category = "Mesh", meta = (Tooltip = "Socket name to attach camera to"))
	FName FirstPersonCameraSocketName = TEXT("head");
};