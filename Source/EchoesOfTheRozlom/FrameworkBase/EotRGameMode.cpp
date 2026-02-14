#include "FrameworkBase/EotRGameMode.h"
#include "Characters/EotRHumanCharacter.h"
#include "Player/EotRPlayerController.h"

AEotRGameMode::AEotRGameMode()
{
	DefaultPawnClass = AEotRHumanCharacter::StaticClass();
	PlayerControllerClass = AEotRPlayerController::StaticClass();
}
