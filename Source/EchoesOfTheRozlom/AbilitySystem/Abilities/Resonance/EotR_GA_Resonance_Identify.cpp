// © 2025 RadZib. All rights reserved.

#include "Abilities/Resonance/EotR_GA_Resonance_Identify.h"
#include "Kismet/GameplayStatics.h"
#include "Anomalies/Actors/EotRAnomalyAreaBase.h"
#include "Interfaces/EotRIdentifiableAnomaly.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/EotRResonanceSet.h"


UGA_Resonance_Identify::UGA_Resonance_Identify()
{
	// Each player runs its own instance; Identify is hold-and-release style
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	// Optional: block Identify if the player is extremely stressed
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Resonance.Overstressed")));

	// Client can predict activation to reduce input delay
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UGA_Resonance_Identify::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	// If activation cost fails, abort ability
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// First scan immediately
	PerformScan();

	// Start repeating scans every X seconds
	if (ActorInfo->AvatarActor.IsValid())
	{
		ActorInfo->AvatarActor->GetWorldTimerManager().SetTimer(
			ScanTimerHandle,
			this,
			&UGA_Resonance_Identify::PerformScan,
			ScanInterval,
			true
		);
	}
}

void UGA_Resonance_Identify::PerformScan()
{
	AActor* Player = GetAvatarActorFromActorInfo();
	if (!Player) return;

	// Temporary array for overlap result
	TArray<AActor*> FoundActors;

	// We do NOT filter using object types because maps vary.
	// Empty array = allow all.
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	// No actors to ignore
	TArray<AActor*> Ignore;

	// --- 1. Perform the actual scan ---
	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		Player->GetActorLocation(),
		ScanRadius,
		ObjectTypes,                        // empty = match all
		AEotRAnomalyAreaBase::StaticClass(), // only return anomalies
		Ignore,
		FoundActors
	);

	// --- 2. Remove highlight from any anomaly no longer found ---
	for (TWeakObjectPtr<AActor>& Old : ActiveHighlights)
	{
		if (!Old.IsValid() || !FoundActors.Contains(Old.Get()))
		{
			if (Old.IsValid() && Old->GetClass()->ImplementsInterface(
				UEotRIdentifiableAnomaly::StaticClass()))
			{
				IEotRIdentifiableAnomaly::Execute_OnIdentifiedStop(
					Old.Get(), Player);
			}
		}
	}

	// Clear and rebuild active list
	ActiveHighlights.Empty();

	// --- 3. Highlight currently found anomalies ---
	for (AActor* A : FoundActors)
	{
		if (!A) continue;

		if (A->GetClass()->ImplementsInterface(
			UEotRIdentifiableAnomaly::StaticClass()))
		{
			IEotRIdentifiableAnomaly::Execute_OnIdentifiedStart(
				A, Player);

			ActiveHighlights.Add(A);
		}
	}

	// --- 4. Stress cost tick ---
	ApplyStress();

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC || !StressTickEffect)
	{
		return;
	}

	// Apply Stress GE every scan tick
	ASC->ApplyGameplayEffectToSelf(
		StressTickEffect.GetDefaultObject(),
		1.f,
		ASC->MakeEffectContext()
	);

	// Read Stress values
	const float CurrentStress =
		ASC->GetNumericAttribute(UEotRResonanceSet::GetStressAttribute());

	const float MaxStress =
		ASC->GetNumericAttribute(UEotRResonanceSet::GetStressMaxAttribute());

	// If overstressed → cancel Identify
	if (MaxStress > 0.f && CurrentStress >= MaxStress * OverstressThreshold)
	{
		ASC->AddLooseGameplayTag(
			FGameplayTag::RequestGameplayTag(TEXT("State.Resonance.Stressed"))
		);

		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}

}

void UGA_Resonance_Identify::ApplyStress()
{
	// Stress system will come later.
	// For now, we make a placeholder comment:
	//
	// UAbilitySystemBlueprintLibrary::ApplyGameplayEffectToOwner(...)
	//
	// Or modify AttributeSet_Resonance once implemented.
}

void UGA_Resonance_Identify::ClearHighlights()
{
	for (auto& A : ActiveHighlights)
	{
		if (A.IsValid() &&
			A->GetClass()->ImplementsInterface(UEotRIdentifiableAnomaly::StaticClass()))
		{
			IEotRIdentifiableAnomaly::Execute_OnIdentifiedStop(
				A.Get(),
				GetAvatarActorFromActorInfo()
			);
		}
	}
	ActiveHighlights.Empty();
}

void UGA_Resonance_Identify::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	// Stop periodic scanning
	if (ActorInfo->AvatarActor.IsValid())
	{
		ActorInfo->AvatarActor->GetWorldTimerManager().ClearTimer(ScanTimerHandle);
	}

	// Remove all highlight VFX
	ClearHighlights();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
