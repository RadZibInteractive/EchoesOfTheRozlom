// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "EotRGameInstanceSubsystem.generated.h"

class USoundBase;

UCLASS()
class ECHOESOFTHEROZLOM_API UEotRGameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// UGameInstanceSubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// Sound
	USoundBase* GetSoundByTag(const FGameplayTag& Tag) const;

private:
	// Sound
	UPROPERTY(Transient)
	TMap<FGameplayTag, TObjectPtr<USoundBase>> SoundByTag;
};