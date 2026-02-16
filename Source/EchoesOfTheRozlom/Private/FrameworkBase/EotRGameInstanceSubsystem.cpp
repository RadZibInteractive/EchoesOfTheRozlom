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

	UEotRSoundTagConfig* SoundTagConfig = Settings->DefaultSoundTagConfig.LoadSynchronous();

	for (const FEotRTaggedSound& Entry : SoundTagConfig->TaggedSounds)
	{
		if (!Entry.InputTag.IsValid() || !Entry.Sound)
		{
			continue;
		}

		if (!SoundByTag.Contains(Entry.InputTag))
		{
			SoundByTag.Add(Entry.InputTag, Entry.Sound);
		}
	}
}

USoundBase* UEotRGameInstanceSubsystem::GetSoundByTag(const FGameplayTag& Tag) const
{
	if (!Tag.IsValid())
	{
		return nullptr;
	}

	if (const TObjectPtr<USoundBase>* Found = SoundByTag.Find(Tag))
	{
		return Found->Get();
	}

	return nullptr;
}