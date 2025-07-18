// Copyright Epic Games, Inc. All Rights Reserved.


#include "AI/EotREnvQueryContext_Target.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EotRAIController.h"

void UEotREnvQueryContext_Target::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	// get the controller from the query instance
	if (AEotRAIController* Controller = Cast<AEotRAIController>(QueryInstance.Owner))
	{
		// ensure the target is valid
		if (IsValid(Controller->GetCurrentTarget()))
		{
			// add the controller's target actor to the context
			UEnvQueryItemType_Actor::SetContextHelper(ContextData, Controller->GetCurrentTarget());
		}
	}

}
