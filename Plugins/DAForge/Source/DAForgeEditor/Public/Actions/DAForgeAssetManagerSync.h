#pragma once

#include "CoreMinimal.h"

struct FForgeCategoryRule;

class FDAForgeAssetManagerSync
{
public:
	static void SyncToAssetManager(const TArray<FForgeCategoryRule>& Categories);
};
