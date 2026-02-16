// © 2026 RadZib. All rights reserved.

#include "UI/DAForgeMenu.h"

#include "ToolMenus.h"
#include "ContentBrowserMenuContexts.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/SWindow.h"

#include "Settings/DAForgeSettings.h"
#include "UI/DAForgeCreateDialog.h"
#include "Actions/DAForgeAssetCreator.h"

#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

namespace DAForge::Menu
{
	static const FName OwnerName(TEXT("DAForgeEditor"));
	static const FName MenuName_AddNew(TEXT("ContentBrowser.AddNewContextMenu"));
	static const FName SectionName(TEXT("DAForge"));

	static void Toast(const FText& Text, SNotificationItem::ECompletionState State)
	{
		FNotificationInfo Info(Text);
		Info.bFireAndForget = true;
		Info.ExpireDuration = (State == SNotificationItem::CS_Fail) ? 4.0f : 2.0f;
		Info.FadeInDuration = 0.05f;
		Info.FadeOutDuration = 0.15f;
		Info.bUseThrobber = false;

		TSharedPtr<SNotificationItem> Item = FSlateNotificationManager::Get().AddNotification(Info);
		if (Item.IsValid())
		{
			Item->SetCompletionState(State);
			Item->ExpireAndFadeout();
		}
	}

	static FString GetClickedPath(const FToolMenuContext& MenuContext)
	{
		if (const UContentBrowserDataMenuContext_AddNewMenu* Ctx = MenuContext.FindContext<UContentBrowserDataMenuContext_AddNewMenu>())
		{
			if (Ctx->SelectedPaths.Num() > 0)
			{
				return Ctx->SelectedPaths[0].ToString();
			}
		}
		return TEXT("/Game");
	}

	static void OpenCreateDialogForPath(const FString& ClickedPath)
	{
		const UDAForgeSettings* S = GetDefault<UDAForgeSettings>();
		if (!S || S->Categories.Num() == 0)
		{
			Toast(
				FText::FromString(TEXT("DA Forge: No categories configured (Project Settings -> Plugins -> DAForge).")),
				SNotificationItem::CS_Fail);
			return;
		}

		TArray<const FForgeCategoryRule*> Rules;
		Rules.Reserve(S->Categories.Num());
		for (const FForgeCategoryRule& Rule : S->Categories)
		{
			Rules.Add(&Rule);
		}

		TSharedRef<SWindow> Window = SNew(SWindow)
			.Title(FText::FromString(TEXT("DA Forge - Create Asset")))
			.ClientSize(FVector2D(600.f, 280.f))
			.SizingRule(ESizingRule::UserSized)
			.SupportsMinimize(false)
			.SupportsMaximize(false);

		TSharedPtr<SDAForgeCreateDialog> Dialog;

		const auto CloseWindow = [W = TWeakPtr<SWindow>(Window)]()
			{
				if (TSharedPtr<SWindow> Pinned = W.Pin())
				{
					Pinned->RequestDestroyWindow();
				}
			};

		const auto OnCancel = FSimpleDelegate::CreateLambda([CloseWindow]()
			{
				CloseWindow();
			});

		const auto OnCreate = FSimpleDelegate::CreateLambda([CloseWindow, &Dialog, ClickedPath]()
			{
				if (!Dialog.IsValid())
				{
					return;
				}

				const FForgeCategoryRule* Rule = Dialog->GetSelectedRule();
				if (!Rule)
				{
					Toast(FText::FromString(TEXT("DA Forge: Please select a type.")), SNotificationItem::CS_Fail);
					return;
				}

				const DAForge::FCreateResult Result =
					DAForge::FDAForgeAssetCreator::CreateAsset(*Rule, ClickedPath, Dialog->GetSuffix(), Dialog->GetOverrideIcon());

				if (!Result.bSucceeded())
				{
					Toast(Result.Error.IsEmpty() ? FText::FromString(TEXT("DA Forge: Create failed.")) : Result.Error,
						SNotificationItem::CS_Fail);
					return;
				}

				Toast(FText::FromString(TEXT("DA Forge: Asset created.")), SNotificationItem::CS_Success);
				CloseWindow();
			});

		Window->SetContent(
			SAssignNew(Dialog, SDAForgeCreateDialog)
			.CategoryRules(Rules)
			.TargetPath(ClickedPath)
			.OnCancel(OnCancel)
			.OnCreate(OnCreate)
		);

		FSlateApplication::Get().AddModalWindow(Window, nullptr);
	}

	static void RegisterMenus_Impl()
	{
		UToolMenus* ToolMenus = UToolMenus::Get();
		if (!ToolMenus)
		{
			return;
		}

		UToolMenu* Menu = ToolMenus->ExtendMenu(MenuName_AddNew);
		if (!Menu)
		{
			return;
		}

		FToolMenuOwnerScoped Owner(OwnerName);

		FToolMenuSection& Section = Menu->FindOrAddSection(SectionName, FText::FromString(TEXT("DAForge")));

		Section.AddMenuEntry(
			NAME_None,
			FText::FromString(TEXT("DA Forge: Create Asset...")),
			FText::FromString(TEXT("Create a Forge DataAsset using DA Forge rules.")),
			FSlateIcon(),
			FToolMenuExecuteAction::CreateLambda([](const FToolMenuContext& Ctx)
				{
					OpenCreateDialogForPath(GetClickedPath(Ctx));
				})
		);
	}

	static void UnregisterMenus_Impl()
	{
		if (UToolMenus* ToolMenus = UToolMenus::Get())
		{
			ToolMenus->UnregisterOwnerByName(OwnerName);
		}
	}
}

namespace DAForge
{
	void RegisterMenus()
	{
		UToolMenus::RegisterStartupCallback(
			FSimpleMulticastDelegate::FDelegate::CreateStatic(&DAForge::Menu::RegisterMenus_Impl)
		);
	}

	void UnregisterMenus()
	{
		DAForge::Menu::UnregisterMenus_Impl();
	}
}