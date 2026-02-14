#include "Player/EotRPlayerController.h"
#include "FrameworkBase/EotRLocalPlayerSubsystem.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"

void AEotRPlayerController::InitInputSystem()
{
	Super::InitInputSystem();

	const ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (!LocalPlayer)
	{
		return;
	}

	const UEotRLocalPlayerSubsystem* LocalPlayerSubsystem = LocalPlayer->GetSubsystem<UEotRLocalPlayerSubsystem>();
	if (!LocalPlayerSubsystem)
	{
		return;
	}

	const UInputMappingContext* InputMappingContext = LocalPlayerSubsystem->GetInputMappingContext();
	if (!InputMappingContext)
	{
		return;
	}

	if (UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
	{
		EnhancedInputSubsystem->AddMappingContext(InputMappingContext, 0);
	}
}