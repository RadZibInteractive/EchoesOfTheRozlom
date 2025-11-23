// © 2025 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "EotRResonanceSet.generated.h"

/**
 * Handy helper macro for getters/setters for GAS attributes.
 */
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

 /**
  * AttributeSet for the Resonance System:
  *  - ResonanceEnergy: resource used for abilities (Absorb, Manipulate, etc.)
  *  - Stress: mental load from using Resonance (Identify, Absorb, Manipulate)
  */
UCLASS()
class ECHOESOFTHEROZLOM_API UEotRResonanceSet : public UAttributeSet
{
	GENERATED_BODY()

public:

	UEotRResonanceSet() {}

	// --------------------------------------------------------------------
	// RESONANCE ENERGY  
	// --------------------------------------------------------------------

	/** Current amount of Resonance Energy */
	UPROPERTY(BlueprintReadOnly, Category = "Resonance", ReplicatedUsing = OnRep_ResonanceEnergy)
	FGameplayAttributeData ResonanceEnergy;
	ATTRIBUTE_ACCESSORS(UEotRResonanceSet, ResonanceEnergy);

	/** Maximum Resonance Energy */
	UPROPERTY(BlueprintReadOnly, Category = "Resonance", ReplicatedUsing = OnRep_ResonanceEnergyMax)
	FGameplayAttributeData ResonanceEnergyMax;
	ATTRIBUTE_ACCESSORS(UEotRResonanceSet, ResonanceEnergyMax);


	// --------------------------------------------------------------------
	// STRESS  
	// --------------------------------------------------------------------

	/** Current Stress level */
	UPROPERTY(BlueprintReadOnly, Category = "Resonance", ReplicatedUsing = OnRep_Stress)
	FGameplayAttributeData Stress;
	ATTRIBUTE_ACCESSORS(UEotRResonanceSet, Stress);

	/** Maximum Stress allowed */
	UPROPERTY(BlueprintReadOnly, Category = "Resonance", ReplicatedUsing = OnRep_StressMax)
	FGameplayAttributeData StressMax;
	ATTRIBUTE_ACCESSORS(UEotRResonanceSet, StressMax);


	// --------------------------------------------------------------------
	// Replication handlers
	// --------------------------------------------------------------------
	UFUNCTION() void OnRep_ResonanceEnergy(const FGameplayAttributeData& OldValue)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UEotRResonanceSet, ResonanceEnergy, OldValue);
	}

	UFUNCTION() void OnRep_ResonanceEnergyMax(const FGameplayAttributeData& OldValue)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UEotRResonanceSet, ResonanceEnergyMax, OldValue);
	}

	UFUNCTION() void OnRep_Stress(const FGameplayAttributeData& OldValue)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UEotRResonanceSet, Stress, OldValue);
	}

	UFUNCTION() void OnRep_StressMax(const FGameplayAttributeData& OldValue)
	{
		GAMEPLAYATTRIBUTE_REPNOTIFY(UEotRResonanceSet, StressMax, OldValue);
	}


	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;


	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
