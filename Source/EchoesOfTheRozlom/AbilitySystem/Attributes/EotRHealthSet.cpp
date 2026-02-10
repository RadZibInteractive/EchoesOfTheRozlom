#include "AbilitySystem/Attributes/EotRHealthSet.h"
#include "Net/UnrealNetwork.h"

void UEotRHealthSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UEotRHealthSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UEotRHealthSet, MaxHealth, COND_None, REPNOTIFY_Always);
}
