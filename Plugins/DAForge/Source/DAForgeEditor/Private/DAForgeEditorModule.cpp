// © 2026 RadZib. All rights reserved.

#include "DAForgeEditorModule.h"

#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "ThumbnailRendering/ThumbnailManager.h"

#include "ForgeDataAsset.h"
#include "Settings/DAForgeSettings.h"
#include "Settings/DAForgeSettingsCustomization.h"
#include "UI/DAForgeMenu.h"
#include "UI/DAForgeThumbnailRenderer.h"

void FDAForgeEditorModule::StartupModule()
{
	DAForge::RegisterMenus();

	RegisterPropertyCustomizations();
	RegisterThumbnailRenderer();
}

void FDAForgeEditorModule::ShutdownModule()
{
	DAForge::UnregisterMenus();

	UnregisterPropertyCustomizations();
	UnregisterThumbnailRenderer();
}

void FDAForgeEditorModule::RegisterPropertyCustomizations()
{
	FPropertyEditorModule& PropModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	PropModule.RegisterCustomClassLayout(
		UDAForgeSettings::StaticClass()->GetFName(),
		FOnGetDetailCustomizationInstance::CreateStatic(&FDAForgeSettingsCustomization::MakeInstance)
	);

	PropModule.NotifyCustomizationModuleChanged();
	bPropertyLayoutRegistered = true;
}

void FDAForgeEditorModule::UnregisterPropertyCustomizations()
{
	if (!bPropertyLayoutRegistered)
	{
		return;
	}

	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropModule.UnregisterCustomClassLayout(UDAForgeSettings::StaticClass()->GetFName());
		PropModule.NotifyCustomizationModuleChanged();
	}

	bPropertyLayoutRegistered = false;
}

void FDAForgeEditorModule::RegisterThumbnailRenderer()
{
	UThumbnailManager::Get().RegisterCustomRenderer(
		UForgeDataAsset::StaticClass(),
		UDAForgeThumbnailRenderer::StaticClass()
	);

	bThumbnailRendererRegistered = true;
}

void FDAForgeEditorModule::UnregisterThumbnailRenderer()
{
	if (!bThumbnailRendererRegistered)
	{
		return;
	}

	if (UObjectInitialized())
	{
		UThumbnailManager::Get().UnregisterCustomRenderer(UForgeDataAsset::StaticClass());
	}

	bThumbnailRendererRegistered = false;
}

IMPLEMENT_MODULE(FDAForgeEditorModule, DAForgeEditor)
