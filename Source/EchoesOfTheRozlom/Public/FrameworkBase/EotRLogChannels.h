// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Logging/LogMacros.h"

class UObject;

ECHOESOFTHEROZLOM_API DECLARE_LOG_CATEGORY_EXTERN(LogEotR, Log, All);
ECHOESOFTHEROZLOM_API DECLARE_LOG_CATEGORY_EXTERN(LogEotRAbilitySystem, Log, All);

ECHOESOFTHEROZLOM_API FString GetClientServerContextString(UObject* ContextObject = nullptr);
