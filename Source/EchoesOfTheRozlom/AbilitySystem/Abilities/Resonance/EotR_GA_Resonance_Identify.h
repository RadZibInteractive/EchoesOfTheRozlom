// © 2025 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/EotRGameplayAbility.h" 
#include "EotR_GA_Resonance_Identify.generated.h"

// Forward declarations
class AEotRAnomalyAreaBase;

/**
 * Gameplay Ability — Resonance: Identify
 *
 * This ability lets the player "sense" anomalies from a distance.
 * It does NOT apply GE, does NOT enter anomalies, and does NOT cause damage.
 * It only highlights anomalies via the IEotRIdentifiableAnomaly interface.
 */
UCLASS(Blueprintable)
class ECHOESOFTHEROZLOM_API UGA_Resonance_Identify : public UEotRGameplayAbility
{
	GENERATED_BODY()

public:

	UGA_Resonance_Identify();


	/** How far the Identify ability scans around the player */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Identify")
	float ScanRadius = 2500.f;

	/** How frequently the ability rescans while active (in seconds) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Identify")
	float ScanInterval = 0.25f;

	/** How much Stress is added per scan tick (placeholder value; scaled later) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Identify")
	float StressPerTick = 1.5f;

protected:

	/** Timer for periodic scanning */
	FTimerHandle ScanTimerHandle;

	/** GameplayEffect used to add Stress while Identify is active */
	UPROPERTY(EditDefaultsOnly, Category = "Resonance|Stress")
	TSubclassOf<UGameplayEffect> StressTickEffect;

	/** Percentage of StressMax at which Identify auto-cancels */
	UPROPERTY(EditDefaultsOnly, Category = "Resonance|Stress")
	float OverstressThreshold = 0.85f;

	/** All anomalies currently highlighted by Identify */
	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> ActiveHighlights;

	//~ Begin UGameplayAbility interface
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	) override;
	//~ End UGameplayAbility interface

	/** Performs one scan and highlights anomalies */
	void PerformScan();

	/** Removes all currently active highlights */
	void ClearHighlights();

	/** Optional: applies stress gain */
	void ApplyStress();
};
