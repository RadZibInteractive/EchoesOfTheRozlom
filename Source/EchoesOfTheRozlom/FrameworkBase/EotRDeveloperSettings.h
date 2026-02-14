#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "EotRDeveloperSettings.generated.h"

class UInputMappingContext;
class UEotRInputTagConfig;
class UCurveFloat;
class UEotRSoundTagConfig;

UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Default"))
class ECHOESOFTHEROZLOM_API UEotRDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category = "Input")
	TSoftObjectPtr<UInputMappingContext> DefaultInputMappingContext = nullptr;

	UPROPERTY(Config, EditAnywhere, Category = "Input")
	TSoftObjectPtr<UEotRInputTagConfig> DefaultInputTagConfig = nullptr;

	UPROPERTY(Config, EditAnywhere, Category = "Movement")
	TSoftObjectPtr<UCurveFloat> DefaultStrafeSpeedMapCurve = nullptr;

	UPROPERTY(Config, EditAnywhere, Category = "Sound")
	TSoftObjectPtr<UEotRSoundTagConfig> DefaultSoundTagConfig = nullptr;
};