// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Data/ForgeTypes.h"
#include "ForgeAnomalyProfile.generated.h"

UCLASS(BlueprintType)
class FORGECORE_API UForgeAnomalyProfile : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Anomaly|Identity")
	FName ProfileId;

	UPROPERTY(EditDefaultsOnly, Category = "Anomaly|Identity")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, Category = "Anomaly|Type")
	EForgeAnomalyType AnomalyType = EForgeAnomalyType::Gravitational;

	UPROPERTY(EditDefaultsOnly, Category = "Anomaly|Parameters", meta = (ClampMin = "0.0"))
	float Radius = 500.f;

	// Damage falloff from center (0=no falloff, 1=full falloff at edge)
	UPROPERTY(EditDefaultsOnly, Category = "Anomaly|Parameters", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Falloff = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Anomaly|Parameters", meta = (ClampMin = "0.0"))
	float DamagePerSecond = 10.f;

	UPROPERTY(EditDefaultsOnly, Category = "Anomaly|Parameters")
	EForgeDamageType DamageType = EForgeDamageType::Rozlom;

	UPROPERTY(EditDefaultsOnly, Category = "Anomaly|Parameters")
	bool bBlocksPlayerPath = false;

	// Reference to Niagara particle system for visuals (soft path to avoid Niagara module dependency in ForgeCore)
	UPROPERTY(EditDefaultsOnly, Category = "Anomaly|Visuals")
	FSoftObjectPath VFXSystem;

	UPROPERTY(EditDefaultsOnly, Category = "Anomaly|Visuals")
	FSoftObjectPath AmbientSound;

	// Rozlom energy recharge multiplier for weapons inside this anomaly
	UPROPERTY(EditDefaultsOnly, Category = "Anomaly|Gameplay", meta = (ClampMin = "0.0"))
	float RozlomRechargeMultiplier = 1.f;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("ForgeAnomalyProfile", GetFName());
	}
};
