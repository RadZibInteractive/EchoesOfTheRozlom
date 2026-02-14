#include "FrameworkBase/EotRGameInstanceSubsystem.h"

#include "FrameworkBase/EotRDeveloperSettings.h"
#include "Data/DataAssets/EotRSoundTagConfig.h"
#include "GameplayTagContainer.h"
#include "Sound/SoundBase.h"

void UEotRGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const UEotRDeveloperSettings* Settings = GetDefault<UEotRDeveloperSettings>();
	if (!Settings)
	{
		return;
	}

	SoundTagConfig = Settings->DefaultSoundTagConfig.LoadSynchronous();
}

USoundBase* UEotRGameInstanceSubsystem::GetSoundByTag(const FGameplayTag& Tag) const
{
	if (!SoundTagConfig)
	{
		return nullptr;
	}

	for (const FEotRTaggedSound& Entry : SoundTagConfig->TaggedSounds)
	{
		if (Entry.InputTag == Tag)
		{
			return Entry.Sound;
		}
	}

	return nullptr;
}