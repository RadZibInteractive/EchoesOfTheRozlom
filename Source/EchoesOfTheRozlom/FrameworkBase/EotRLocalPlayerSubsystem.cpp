#include "FrameworkBase/EotRLocalPlayerSubsystem.h"

#include "FrameworkBase/EotRDeveloperSettings.h"
#include "InputMappingContext.h"
#include "Data/DataAssets/EotRInputTagConfig.h"

void UEotRLocalPlayerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const UEotRDeveloperSettings* Settings = GetDefault<UEotRDeveloperSettings>();
	if (!Settings)
	{
		return;
	}

	InputMappingContext = Settings->DefaultInputMappingContext.LoadSynchronous();
	InputTagConfig = Settings->DefaultInputTagConfig.LoadSynchronous();
}