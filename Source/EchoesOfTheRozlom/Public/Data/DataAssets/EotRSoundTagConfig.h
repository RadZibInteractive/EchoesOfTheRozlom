#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "EotRSoundTagConfig.generated.h"

class USoundBase;

USTRUCT()
struct ECHOESOFTHEROZLOM_API FEotRTaggedSound
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USoundBase> Sound = nullptr;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputTag;
};

UCLASS()
class ECHOESOFTHEROZLOM_API UEotRSoundTagConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TArray<FEotRTaggedSound> TaggedSounds;
};