// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"

// Creates a set of starter Forge data assets under /Game/ForgeContent/Starter/
// if they don't already exist.  Safe to call multiple times — existing assets
// are skipped.  Returns the number of assets actually created (0 = already up to date).
class FORGEEDITOR_API FForgeStarterContent
{
public:
	static int32 CreateAll();

private:
	static const FString RootPath;   // "/Game/ForgeContent/Starter"

	template<typename T>
	static T* CreateAsset(const FString& AssetName);

	static void CreateNPCProfiles();
	static void CreateAnomalyProfiles();
	static void CreateDialogue();
	static void CreateQuest();

	static bool AssetExists(const FString& PackagePath);
	static void SaveAndRegister(UObject* Asset, UPackage* Package);
};
