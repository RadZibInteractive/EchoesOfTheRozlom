// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "NativeGameplayTags.h"

namespace EotRGameplayTags
{
	ECHOESOFTHEROZLOM_API	FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString = false);

	// Declare all of the custom native tags that EotR will use

	ECHOESOFTHEROZLOM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Damage);
	ECHOESOFTHEROZLOM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Heal);

	// ==== Anomalies / Resonance ===============================================
	ECHOESOFTHEROZLOM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Anomaly_Type_Cloud);
	ECHOESOFTHEROZLOM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Anomaly_Type_Energy);
	ECHOESOFTHEROZLOM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Anomaly_Type_Gravity);
	ECHOESOFTHEROZLOM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Anomaly_Type_Biochemical);

	ECHOESOFTHEROZLOM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Anomaly_Stabilized);

	ECHOESOFTHEROZLOM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Damage_Anomaly_Toxic);
	ECHOESOFTHEROZLOM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Block_HealFull);

	ECHOESOFTHEROZLOM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Resonance_Action_Stabilize);
	ECHOESOFTHEROZLOM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Resonance_Action_Channel);

	ECHOESOFTHEROZLOM_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GC_Anomaly_Toxic_Inside);
};
