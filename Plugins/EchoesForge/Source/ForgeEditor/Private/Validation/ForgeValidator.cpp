// © 2026 RadZib. All rights reserved.

#include "Validation/ForgeValidator.h"
#include "Actors/ForgeWorldActors.h"
#include "Data/ForgeNPCProfile.h"     // UForgeNPCProfile, EForgeNPCArchetype
#include "Data/ForgeAnomalyProfile.h" // UForgeAnomalyProfile, EForgeAnomalyType
#include "Data/ForgeDialogueData.h"   // full type required by TSoftObjectPtr<UForgeDialogueData>::IsValid()
#include "EngineUtils.h"              // TActorIterator

// ─── Public entry point ───────────────────────────────────────────────────────

TArray<FForgeValidationResult> FForgeValidator::ValidateWorld(UWorld* World)
{
	TArray<FForgeValidationResult> Results;

	if (!World)
	{
		Results.Add({ EForgeValidationStatus::Error, TEXT("—"),
			TEXT("No active world. Open a level first.") });
		return Results;
	}

	CheckNPCActors    (World, Results);
	CheckAnomalyActors(World, Results);
	CheckEnemyActors  (World, Results);
	CheckPropActors   (World, Results);

	if (Results.IsEmpty())
	{
		Results.Add({ EForgeValidationStatus::Valid, TEXT("World"),
			TEXT("All Forge actors are valid.") });
	}

	return Results;
}

// ─── NPC checks ───────────────────────────────────────────────────────────────

void FForgeValidator::CheckNPCActors(UWorld* World, TArray<FForgeValidationResult>& Out)
{
	for (TActorIterator<AForgeNPCActor> It(World); It; ++It)
	{
		const FString Label = It->GetActorLabel();

		if (!It->Profile)
		{
			Out.Add({ EForgeValidationStatus::Error, Label,
				TEXT("NPC has no Profile assigned.") });
			continue;
		}

		UForgeNPCProfile* P = It->Profile;

		// QuestGiver archetype should have an associated quest
		if (P->Archetype == EForgeNPCArchetype::QuestGiver && P->AssociatedQuest.IsNull())
		{
			Out.Add({ EForgeValidationStatus::Warning, Label,
				FString::Printf(TEXT("NPC '%s' archetype is QuestGiver but has no AssociatedQuest set."),
				*P->DisplayName.ToString()) });
		}

		// Profile references a dialogue — verify soft ptr is not broken
		if (!P->DialogueData.IsNull() && !P->DialogueData.IsValid())
		{
			Out.Add({ EForgeValidationStatus::Warning, Label,
				FString::Printf(TEXT("NPC '%s' DialogueData soft reference is unresolvable (asset deleted?)."),
				*P->DisplayName.ToString()) });
		}

		// Merchant archetype should at minimum have a display name
		if (P->Archetype == EForgeNPCArchetype::Merchant && P->DisplayName.IsEmpty())
		{
			Out.Add({ EForgeValidationStatus::Warning, Label,
				TEXT("Merchant NPC has no DisplayName — shop UI will show nothing.") });
		}
	}
}

// ─── Anomaly checks ───────────────────────────────────────────────────────────

void FForgeValidator::CheckAnomalyActors(UWorld* World, TArray<FForgeValidationResult>& Out)
{
	for (TActorIterator<AForgeAnomalyActor> It(World); It; ++It)
	{
		const FString Label = It->GetActorLabel();

		if (!It->Profile)
		{
			Out.Add({ EForgeValidationStatus::Error, Label,
				TEXT("Anomaly has no Profile assigned.") });
			continue;
		}

		UForgeAnomalyProfile* P = It->Profile;

		const float EffectiveRadius = It->RadiusOverride > 0.f
			? It->RadiusOverride
			: P->Radius;

		if (EffectiveRadius <= 0.f)
		{
			Out.Add({ EForgeValidationStatus::Warning, Label,
				TEXT("Anomaly radius is 0 — it will have no effect in game.") });
		}

		// Balance check: Rozlom recharge multiplier above 5 is a design red flag
		if (P->RozlomRechargeMultiplier > 5.f)
		{
			Out.Add({ EForgeValidationStatus::Warning, Label,
				FString::Printf(TEXT("Anomaly '%s' RozlomRechargeMultiplier is %.1f — "
					"values above 5× may unbalance weapon energy economy."),
					*P->DisplayName.ToString(), P->RozlomRechargeMultiplier) });
		}

		// Blocking anomaly with zero DPS has no gameplay effect
		if (P->bBlocksPlayerPath && P->DamagePerSecond <= 0.f)
		{
			Out.Add({ EForgeValidationStatus::Warning, Label,
				TEXT("Anomaly blocks player path but deals no damage — "
					"consider adding DamagePerSecond or removing path block.") });
		}
	}
}

// ─── Enemy checks ─────────────────────────────────────────────────────────────

void FForgeValidator::CheckEnemyActors(UWorld* World, TArray<FForgeValidationResult>& Out)
{
	for (TActorIterator<AForgeEnemyActor> It(World); It; ++It)
	{
		const FString Label = It->GetActorLabel();

		if (!It->EnemyProfile)
		{
			Out.Add({ EForgeValidationStatus::Error, Label,
				TEXT("Enemy has no Profile assigned.") });
		}

		if (It->SpawnCount < 1)
		{
			Out.Add({ EForgeValidationStatus::Warning, Label,
				TEXT("Enemy SpawnCount < 1 — will not spawn anything.") });
		}

		// Patrol enabled but not enough waypoints to form a path
		if (It->bPatrolEnabled && It->PatrolWaypoints.Num() < 2)
		{
			Out.Add({ EForgeValidationStatus::Warning, Label,
				FString::Printf(TEXT("Enemy has patrol enabled but only %d waypoint(s) — "
					"need at least 2 to form a route."),
					It->PatrolWaypoints.Num()) });
		}

		// Loop patrol with only one waypoint is effectively no patrol
		if (It->bLoopPatrol && It->PatrolWaypoints.Num() == 1)
		{
			Out.Add({ EForgeValidationStatus::Warning, Label,
				TEXT("Enemy bLoopPatrol=true but only 1 waypoint — enemy will stand still.") });
		}
	}
}

// ─── Prop checks ──────────────────────────────────────────────────────────────

void FForgeValidator::CheckPropActors(UWorld* World, TArray<FForgeValidationResult>& Out)
{
	for (TActorIterator<AForgePropActor> It(World); It; ++It)
	{
		const FString Label = It->GetActorLabel();

		if (It->PropId.IsNone())
		{
			Out.Add({ EForgeValidationStatus::Warning, Label,
				TEXT("Prop has no PropId — will be ignored by runtime.") });
		}

		if (It->bIsInteractable && It->InteractLabel.IsEmpty())
		{
			Out.Add({ EForgeValidationStatus::Warning, Label,
				TEXT("Interactable prop has no InteractLabel set.") });
		}
	}
}
