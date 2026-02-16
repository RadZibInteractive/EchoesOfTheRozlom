// © 2026 RadZib. All rights reserved.

#include "Settings/DAForgeSettingsCustomization.h"

#include "Settings/DAForgeSettings.h"
#include "Actions/DAForgeAssetManagerSync.h"

#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Misc/MessageDialog.h"

TSharedRef<IDetailCustomization> FDAForgeSettingsCustomization::MakeInstance()
{
	return MakeShared<FDAForgeSettingsCustomization>();
}

void FDAForgeSettingsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	IDetailCategoryBuilder& Cat = DetailBuilder.EditCategory(TEXT("DAForge"));

	Cat.AddCustomRow(FText::FromString(TEXT("DAForge Sync")))
		.WholeRowContent()
		[
			SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SButton)
						.Text(FText::FromString(TEXT("Validate & Sync to Asset Manager")))
						.ToolTipText(FText::FromString(TEXT("Validates DAForge categories and updates Project Settings -> Asset Manager -> Primary Asset Types to Scan.")))
						.OnClicked_Lambda([&DetailBuilder]()
							{
								const UDAForgeSettings* Settings = GetDefault<UDAForgeSettings>();
								if (!Settings)
								{
									FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("DAForge settings not found.")));
									return FReply::Handled();
								}

								FDAForgeAssetManagerSync::SyncToAssetManager(Settings->Categories);

								DetailBuilder.ForceRefreshDetails();
								return FReply::Handled();
							})
				]

			+ SHorizontalBox::Slot()
				.FillWidth(1.f)
				.Padding(12.f, 0.f)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
						.Text(FText::FromString(TEXT("Applies scan directories/types to Asset Manager config.")))
				]
		];
}
