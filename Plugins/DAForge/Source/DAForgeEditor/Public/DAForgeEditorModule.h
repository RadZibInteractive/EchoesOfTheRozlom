// © 2026 RadZib. All rights reserved.

#pragma once

#include "Modules/ModuleManager.h"

/**
 * @class DAForge editor module.
 */
class FDAForgeEditorModule final : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void RegisterPropertyCustomizations();
	void UnregisterPropertyCustomizations();
	void RegisterThumbnailRenderer();
	void UnregisterThumbnailRenderer();

private:
	// Helps avoid double-unregister in some hot-reload / shutdown paths.
	bool bPropertyLayoutRegistered = false;
	bool bThumbnailRendererRegistered = false;
};