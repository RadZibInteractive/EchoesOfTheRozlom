// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/ForgeNPCProfile.h"
#include "Data/ForgeAnomalyProfile.h"
#include "ForgeWorldActors.generated.h"

// ─── NPC ─────────────────────────────────────────────────────────────────────

UCLASS(BlueprintType, Blueprintable, DisplayName = "Forge NPC")
class FORGERUNTIME_API AForgeNPCActor : public AActor
{
	GENERATED_BODY()
public:
	AForgeNPCActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Forge|NPC")
	TObjectPtr<UForgeNPCProfile> Profile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Forge|NPC")
	FName OverrideName;   // empty = use Profile display name
};

// ─── Anomaly ─────────────────────────────────────────────────────────────────

UCLASS(BlueprintType, Blueprintable, DisplayName = "Forge Anomaly")
class FORGERUNTIME_API AForgeAnomalyActor : public AActor
{
	GENERATED_BODY()
public:
	AForgeAnomalyActor();

	// Forge-built anomaly profile (ForgeAnomalyProfile)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Forge|Anomaly")
	TObjectPtr<UForgeAnomalyProfile> Profile;

	// Game-native anomaly profile (e.g. EotRAnomalyProfile from EchoesOfTheRozlom).
	// Takes priority over Profile when set; runtime code casts as needed.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Forge|Anomaly",
		meta = (AllowedClasses = "PrimaryDataAsset"))
	TObjectPtr<UPrimaryDataAsset> EotProfile;

	// 0 = use Profile radius (ignored when EotProfile provides its own shape)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Forge|Anomaly",
		meta = (ClampMin = "0.0"))
	float RadiusOverride = 0.f;
};

// ─── Enemy ───────────────────────────────────────────────────────────────────

UCLASS(BlueprintType, Blueprintable, DisplayName = "Forge Enemy")
class FORGERUNTIME_API AForgeEnemyActor : public AActor
{
	GENERATED_BODY()
public:
	AForgeEnemyActor();

	// Enemy reuses NPC profile — archetype should be EForgeNPCArchetype::Enemy/Boss
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Forge|Enemy")
	TObjectPtr<UForgeNPCProfile> EnemyProfile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Forge|Enemy",
		meta = (ClampMin = "1"))
	int32 SpawnCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Forge|Enemy")
	bool bPatrolEnabled = false;

	// Patrol waypoints in world space — edit via the Forge Inspector patrol panel
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Forge|Enemy|Patrol")
	TArray<FVector> PatrolWaypoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Forge|Enemy|Patrol")
	bool bLoopPatrol = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Forge|Enemy|Patrol",
		meta = (ClampMin = "50.0", ClampMax = "600.0"))
	float WaypointReachRadius = 100.f;
};

// ─── Props ───────────────────────────────────────────────────────────────────

UCLASS(BlueprintType, Blueprintable, DisplayName = "Forge Prop")
class FORGERUNTIME_API AForgePropActor : public AActor
{
	GENERATED_BODY()
public:
	AForgePropActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Forge|Props")
	FName PropId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Forge|Props")
	bool bIsInteractable = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Forge|Props")
	FText InteractLabel;
};
