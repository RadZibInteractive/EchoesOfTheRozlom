// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "EotRVerbMessageHelpers.generated.h"

struct FGameplayCueParameters;
struct FEotRVerbMessage;

class APlayerController;
class APlayerState;
class UObject;
struct FFrame;


UCLASS()
class ECHOESOFTHEROZLOM_API UEotRVerbMessageHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "EotR")
	static APlayerState* GetPlayerStateFromObject(UObject* Object);

	UFUNCTION(BlueprintCallable, Category = "EotR")
	static APlayerController* GetPlayerControllerFromObject(UObject* Object);

	UFUNCTION(BlueprintCallable, Category = "EotR")
	static FGameplayCueParameters VerbMessageToCueParameters(const FEotRVerbMessage& Message);

	UFUNCTION(BlueprintCallable, Category = "EotR")
	static FEotRVerbMessage CueParametersToVerbMessage(const FGameplayCueParameters& Params);
};
