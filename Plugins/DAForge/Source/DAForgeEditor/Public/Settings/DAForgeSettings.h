// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "DAForgeSettings.generated.h"

class UTexture2D;
class UForgeDataAsset;

USTRUCT()
struct FForgeCategoryRule
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Category")
	FName PrimaryAssetType = NAME_None;

	UPROPERTY(EditAnywhere, Category = "Category")
	FText DisplayTitle;

	// Must derive from UForgeDataAsset
	UPROPERTY(EditAnywhere, Category = "Category")
	TSoftClassPtr<UForgeDataAsset> AssetClass;

	UPROPERTY(EditAnywhere, Category = "Category")
	FDirectoryPath RootPath = FDirectoryPath(TEXT("/Game/Data"));

	UPROPERTY(EditAnywhere, Category = "Category")
	TSoftObjectPtr<UTexture2D> DefaultIcon;

	UPROPERTY(EditAnywhere, Category = "Category")
	bool bOpenAfterCreate = true;
};

UCLASS(Config = Editor, DefaultConfig)
class DAFORGEEDITOR_API UDAForgeSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Config, Category = "DAForge")
	TArray<FForgeCategoryRule> Categories;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& E) override
	{
		Super::PostEditChangeProperty(E);
		SaveConfig();
	}
#endif

	virtual FName GetCategoryName() const override { return TEXT("Plugins"); }
	virtual FName GetSectionName() const override { return TEXT("DAForge"); }
	virtual FText GetSectionText() const override { return FText::FromString(TEXT("DAForge")); }
};
