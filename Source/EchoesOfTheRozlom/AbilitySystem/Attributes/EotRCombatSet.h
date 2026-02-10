#pragma once

#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "EotRCombatSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/** Minimal combat set stub */
UCLASS(BlueprintType)
class ECHOESOFTHEROZLOM_API UEotRCombatSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Combat", ReplicatedUsing = OnRep_BaseDamage)
	FGameplayAttributeData BaseDamage;
	ATTRIBUTE_ACCESSORS(UEotRCombatSet, BaseDamage)

		UPROPERTY(BlueprintReadOnly, Category = "Combat", ReplicatedUsing = OnRep_Damage)
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UEotRCombatSet, Damage)

		UFUNCTION()
	void OnRep_BaseDamage(const FGameplayAttributeData& OldValue) {}

	UFUNCTION()
	void OnRep_Damage(const FGameplayAttributeData& OldValue) {}

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
