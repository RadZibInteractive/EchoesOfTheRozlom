// © 2026 RadZib. All rights reserved.

#pragma once

#include "IDetailCustomization.h"

class IDetailLayoutBuilder;

class FDAForgeSettingsCustomization final : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
};
