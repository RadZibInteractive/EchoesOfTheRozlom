// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EotRBaseGameMode.generated.h"

class UEotRPawnData;

UCLASS(abstract)
class ECHOESOFTHEROZLOM_API AEotRBaseGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	AEotRBaseGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~AGameModeBase interface
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	//~End of AGameModeBase interface

	UPROPERTY(EditDefaultsOnly, Category = "Pawn")
	const UEotRPawnData* DefaultPawnData;
};
