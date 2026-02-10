#include "AbilitySystem/Attributes/EotRCombatSet.h"
#include "Net/UnrealNetwork.h"

void UEotRCombatSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UEotRCombatSet, BaseDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UEotRCombatSet, Damage, COND_None, REPNOTIFY_Always);
}
