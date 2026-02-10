#pragma once

#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "EotRHealthSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/** Minimal health set stub */
UCLASS(BlueprintType)
class ECHOESOFTHEROZLOM_API UEotRHealthSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UEotRHealthSet, Health)

		UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UEotRHealthSet, MaxHealth)

		UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue) {}

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue) {}

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
