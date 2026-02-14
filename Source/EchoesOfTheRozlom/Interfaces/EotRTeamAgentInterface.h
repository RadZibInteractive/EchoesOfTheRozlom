#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GenericTeamAgentInterface.h"

#include "EotRTeamAgentInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEotRTeamAgentInterface : public UGenericTeamAgentInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ECHOESOFTHEROZLOM_API IEotRTeamAgentInterface : public IGenericTeamAgentInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	
};
