#pragma once

#include "GameplayTagContainer.h"
#include "EotRVerbMessage.generated.h"

/** Minimal message payload stub used by PlayerState RPC */
USTRUCT(BlueprintType)
struct FEotRVerbMessage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag Verb;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Payload;
};
