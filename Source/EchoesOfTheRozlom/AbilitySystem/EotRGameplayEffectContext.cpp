// Copyright Epic Games, Inc. All Rights Reserved.

#include "EotRGameplayEffectContext.h"

#include "AbilitySystem/EotRAbilitySourceInterface.h"
#include "Engine/HitResult.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

#if UE_WITH_IRIS
#include "Iris/ReplicationState/PropertyNetSerializerInfoRegistry.h"
#include "Serialization/GameplayEffectContextNetSerializer.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(EotRGameplayEffectContext)

class FArchive;

FEotRGameplayEffectContext* FEotRGameplayEffectContext::ExtractEffectContext(struct FGameplayEffectContextHandle Handle)
{
	FGameplayEffectContext* BaseEffectContext = Handle.Get();
	if ((BaseEffectContext != nullptr) && BaseEffectContext->GetScriptStruct()->IsChildOf(FEotRGameplayEffectContext::StaticStruct()))
	{
		return (FEotRGameplayEffectContext*)BaseEffectContext;
	}

	return nullptr;
}

bool FEotRGameplayEffectContext::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	FGameplayEffectContext::NetSerialize(Ar, Map, bOutSuccess);

	// Not serialized for post-activation use:
	// CartridgeID

	return true;
}

#if UE_WITH_IRIS
namespace UE::Net
{
	// Forward to FGameplayEffectContextNetSerializer
	// Note: If FEotRGameplayEffectContext::NetSerialize() is modified, a custom NetSerializesr must be implemented as the current fallback will no longer be sufficient.
	UE_NET_IMPLEMENT_FORWARDING_NETSERIALIZER_AND_REGISTRY_DELEGATES(EotRGameplayEffectContext, FGameplayEffectContextNetSerializer);
}
#endif

void FEotRGameplayEffectContext::SetAbilitySource(const IEotRAbilitySourceInterface* InObject, float InSourceLevel)
{
	AbilitySourceObject = MakeWeakObjectPtr(Cast<const UObject>(InObject));
	//SourceLevel = InSourceLevel;
}

const IEotRAbilitySourceInterface* FEotRGameplayEffectContext::GetAbilitySource() const
{
	return Cast<IEotRAbilitySourceInterface>(AbilitySourceObject.Get());
}

const UPhysicalMaterial* FEotRGameplayEffectContext::GetPhysicalMaterial() const
{
	if (const FHitResult* HitResultPtr = GetHitResult())
	{
		return HitResultPtr->PhysMaterial.Get();
	}
	return nullptr;
}

