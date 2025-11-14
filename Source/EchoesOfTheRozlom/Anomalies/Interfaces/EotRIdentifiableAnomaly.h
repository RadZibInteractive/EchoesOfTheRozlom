// © 2025 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EotRIdentifiableAnomaly.generated.h"

UINTERFACE(BlueprintType)
class ECHOESOFTHEROZLOM_API UEotRIdentifiableAnomaly : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for anomalies that can be “pinged” by the Resonance Identify ability.
 */
class ECHOESOFTHEROZLOM_API IEotRIdentifiableAnomaly
{
	GENERATED_BODY()

public:

	/** Called when the player starts identifying this anomaly. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Resonance|Identify")
	void OnIdentifiedStart(AActor* Identifier);

	/** Called when the player stops identifying this anomaly. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Resonance|Identify")
	void OnIdentifiedStop(AActor* Identifier);
};
