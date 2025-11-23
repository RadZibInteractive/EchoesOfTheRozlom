// © 2025 RadZib. All rights reserved.

#include "AbilitySystem/Attributes/EotRResonanceSet.h"
#include "Net/UnrealNetwork.h"


	// --------------------------------------------------------------------
	// Clamp values after GameplayEffects modify attributes
	// --------------------------------------------------------------------
	void UEotRResonanceSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
	{
		Super::PostGameplayEffectExecute(Data);

		// Clamp Resonance Energy
		ResonanceEnergy.SetCurrentValue(
			FMath::Clamp(ResonanceEnergy.GetCurrentValue(), 0.f, ResonanceEnergyMax.GetCurrentValue())
		);

		// Clamp Stress
		Stress.SetCurrentValue(
			FMath::Clamp(Stress.GetCurrentValue(), 0.f, StressMax.GetCurrentValue())
		);
	}

	void UEotRResonanceSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);

		DOREPLIFETIME_CONDITION_NOTIFY(UEotRResonanceSet, ResonanceEnergy, COND_None, REPNOTIFY_Always);
		DOREPLIFETIME_CONDITION_NOTIFY(UEotRResonanceSet, ResonanceEnergyMax, COND_None, REPNOTIFY_Always);
		DOREPLIFETIME_CONDITION_NOTIFY(UEotRResonanceSet, Stress, COND_None, REPNOTIFY_Always);
		DOREPLIFETIME_CONDITION_NOTIFY(UEotRResonanceSet, StressMax, COND_None, REPNOTIFY_Always);
	}
