// © 2026 RadZib. All rights reserved.

#include "FrameworkBase/EotRGameMode.h"
#include "Characters/EotRHumanCharacter.h"
#include "Characters/Controllers/EotRPlayerController.h"

AEotRGameMode::AEotRGameMode()
{
	DefaultPawnClass = AEotRHumanCharacter::StaticClass();
	PlayerControllerClass = AEotRPlayerController::StaticClass();
}
