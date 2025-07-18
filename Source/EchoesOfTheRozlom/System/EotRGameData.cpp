// Copyright Epic Games, Inc. All Rights Reserved.

#include "EotRGameData.h"
#include "EotRAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EotRGameData)

UEotRGameData::UEotRGameData()
{
}

const UEotRGameData& UEotRGameData::UEotRGameData::Get()
{
	return UEotRAssetManager::Get().GetGameData();
}
