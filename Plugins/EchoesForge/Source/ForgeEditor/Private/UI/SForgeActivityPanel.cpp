// © 2026 RadZib. All rights reserved.

#include "UI/SForgeActivityPanel.h"
#include "Actors/ForgeWorldActors.h"
#include "Data/ForgeNPCProfile.h"
#include "Data/ForgeAnomalyProfile.h"
#include "Utils/ForgeUIStyle.h"
#include "Utils/ForgeStarterContent.h"

#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "AssetRegistry/AssetData.h"

#include "Editor.h"
#include "EditorViewportClient.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Layout/SBox.h"
#include "Styling/AppStyle.h"

#define LOCTEXT_NAMESPACE "ForgeActivityPanel"

// ─── Construct ───────────────────────────────────────────────────────────────

void SForgeActivityPanel::Construct(const FArguments& InArgs)
{
	ActivityType  = InArgs._ActivityType;
	OnActorPlaced = InArgs._OnActorPlaced;

	// BuildLayout() calls GetPanelTitle() / BuildContent() / BuildFooter().
	// BuildContent() calls RebuildForm() which populates FormContainer.
	BuildLayout();
}

// ─── Public setter ────────────────────────────────────────────────────────────

void SForgeActivityPanel::SetActivityType(EForgeSidebarState NewType)
{
	ActivityType    = NewType;
	SelectedAssetPath.Empty();
	SpawnCount      = 1;
	RadiusOverride  = 0.f;
	bIsInteractable = false;
	RebuildForm();
}

// ─── SForgePanel overrides ────────────────────────────────────────────────────

FText SForgeActivityPanel::GetPanelTitle() const
{
	return FText::FromString(ActivityLabel(ActivityType));
}

TSharedRef<SWidget> SForgeActivityPanel::BuildContent()
{
	TSharedRef<SVerticalBox> Content = SNew(SVerticalBox)

		// Asset picker row
		+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 0.f, 0.f, 8.f)
		[ BuildAssetRow() ]

		// Dynamic extras (changes per activity type)
		+ SVerticalBox::Slot().AutoHeight()
		[
			SAssignNew(FormContainer, SVerticalBox)
		];

	// FormContainer is now valid — populate it for the initial activity type
	RebuildForm();

	return Content;
}

TSharedRef<SWidget> SForgeActivityPanel::BuildFooter()
{
	return SNew(SVerticalBox)

		// ▶ Place in World
		+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 0.f, 0.f, 4.f)
		[
			SNew(SForgeButton)
			.Preset(EForgeButtonPreset::Confirm)
			.Label(LOCTEXT("PlaceBtn", "▶  Place in World"))
			.ContentPadding(FMargin(0.f, 9.f))
			.HAlign(HAlign_Center)
			.Bold(true)
			.FontSize(10)
			.OnClicked(this, &SForgeActivityPanel::OnPlaceActor)
		]

		// 📦 Create Starter Assets
		+ SVerticalBox::Slot().AutoHeight()
		[
			SNew(SForgeButton)
			.Preset(EForgeButtonPreset::Custom)
			.NormalColor(FLinearColor(0.06f, 0.10f, 0.18f))   // dark cyan
			.HoverColor (FLinearColor(0.10f, 0.20f, 0.34f))
			.ContentPadding(FMargin(0.f, 5.f))
			.HAlign(HAlign_Center)
			.ToolTipText(LOCTEXT("StarterTip",
				"Generate starter NPC, Anomaly, Dialogue and Quest assets\n"
				"in /Game/ForgeContent/Starter/ (skips any that already exist)"))
			.OnClicked(FOnClicked::CreateLambda([this]() -> FReply
			{
				const int32 N = FForgeStarterContent::CreateAll();
				if (AssetNameLabel.IsValid())
				{
					AssetNameLabel->SetText(N > 0
						? FText::Format(
							LOCTEXT("StarterCreated", "✓ Created {0} starter asset(s)"),
							FText::AsNumber(N))
						: LOCTEXT("StarterExists", "✓ Starter assets already exist"));
				}
				return FReply::Handled();
			}))
			[
				SNew(STextBlock)
				.Text(LOCTEXT("StarterBtn", "📦  Create Starter Assets"))
				.Font(FCoreStyle::GetDefaultFontStyle("Regular", 8))
				.ColorAndOpacity(FSlateColor(FLinearColor(0.45f, 0.75f, 0.92f)))
			]
		];
}

// ─── Form builder ─────────────────────────────────────────────────────────────

void SForgeActivityPanel::RebuildForm()
{
	if (!FormContainer.IsValid()) return;
	FormContainer->ClearChildren();

	TSharedRef<SWidget> Extras = [this]() -> TSharedRef<SWidget>
	{
		switch (ActivityType)
		{
		case EForgeSidebarState::AnomalySetup: return BuildAnomalyExtras();
		case EForgeSidebarState::EnemySetup:   return BuildEnemyExtras();
		case EForgeSidebarState::PropsSetup:   return BuildPropsExtras();
		default:                               return BuildNPCExtras();
		}
	}();

	FormContainer->AddSlot().AutoHeight()[ Extras ];
}

TSharedRef<SWidget> SForgeActivityPanel::BuildAssetRow()
{
	return SNew(SVerticalBox)

		+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 0.f, 0.f, 3.f)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("ProfileLabel", "Data Profile"))
			.Font(FCoreStyle::GetDefaultFontStyle("Bold", 9))
			.ColorAndOpacity(FSlateColor(FLinearColor(0.65f, 0.67f, 0.78f)))
		]

		+ SVerticalBox::Slot().AutoHeight()
		[
			SNew(SHorizontalBox)

			// Asset name display
			+ SHorizontalBox::Slot().FillWidth(1.f).VAlign(VAlign_Center).Padding(0.f, 0.f, 4.f, 0.f)
			[
				SAssignNew(AssetNameLabel, STextBlock)
				.Text(LOCTEXT("NoAsset", "None"))
				.Font(FCoreStyle::GetDefaultFontStyle("Mono", 8))
				.ColorAndOpacity(FSlateColor(FLinearColor(0.50f, 0.52f, 0.60f)))
				.OverflowPolicy(ETextOverflowPolicy::Ellipsis)
			]

			// Browse button
			+ SHorizontalBox::Slot().AutoWidth()
			[
				SNew(SForgeButton)
				.Preset(EForgeButtonPreset::Navy)
				.Label(LOCTEXT("BrowseBtn", "Browse"))
				.ContentPadding(FMargin(8.f, 4.f))
				.FontSize(9)
				.OnClicked(this, &SForgeActivityPanel::OnBrowseAsset)
			]
		];
}

TSharedRef<SWidget> SForgeActivityPanel::BuildNPCExtras()
{
	return SNullWidget::NullWidget;
}

TSharedRef<SWidget> SForgeActivityPanel::BuildAnomalyExtras()
{
	return SNew(SVerticalBox)

		+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 6.f, 0.f, 2.f)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("RadiusLabel", "Radius Override  (0 = profile default)"))
			.Font(FCoreStyle::GetDefaultFontStyle("Regular", 8))
			.ColorAndOpacity(FSlateColor(FLinearColor(0.55f, 0.57f, 0.65f)))
		]

		+ SVerticalBox::Slot().AutoHeight()
		[
			SNew(SSpinBox<float>)
			.MinValue(0.f).MaxValue(10000.f).Delta(50.f)
			.Value_Lambda     ([this]()        { return RadiusOverride; })
			.OnValueChanged_Lambda([this](float V) { RadiusOverride = V; })
		];
}

TSharedRef<SWidget> SForgeActivityPanel::BuildEnemyExtras()
{
	return SNew(SVerticalBox)

		+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 6.f, 0.f, 2.f)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("SpawnCountLabel", "Spawn Count"))
			.Font(FCoreStyle::GetDefaultFontStyle("Regular", 8))
			.ColorAndOpacity(FSlateColor(FLinearColor(0.55f, 0.57f, 0.65f)))
		]

		+ SVerticalBox::Slot().AutoHeight()
		[
			SNew(SSpinBox<int32>)
			.MinValue(1).MaxValue(64).Delta(1)
			.Value_Lambda     ([this]()         { return SpawnCount; })
			.OnValueChanged_Lambda([this](int32 V) { SpawnCount = V;  })
		];
}

TSharedRef<SWidget> SForgeActivityPanel::BuildPropsExtras()
{
	return SNew(SVerticalBox)

		+ SVerticalBox::Slot().AutoHeight().Padding(0.f, 6.f, 0.f, 2.f)
		[
			SNew(SCheckBox)
			.IsChecked_Lambda([this]()
			{
				return bIsInteractable
					? ECheckBoxState::Checked
					: ECheckBoxState::Unchecked;
			})
			.OnCheckStateChanged_Lambda([this](ECheckBoxState S)
			{
				bIsInteractable = (S == ECheckBoxState::Checked);
			})
			[
				SNew(STextBlock)
				.Text(LOCTEXT("InteractLabel", "Interactable"))
				.Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
				.ColorAndOpacity(FSlateColor(FLinearColor(0.75f, 0.77f, 0.85f)))
			]
		];
}

// ─── Browse ───────────────────────────────────────────────────────────────────

FReply SForgeActivityPanel::OnBrowseAsset()
{
	UClass* FilterClass = ProfileClassFor(ActivityType);
	if (!FilterClass) return FReply::Handled();

	// Window must be created before the picker so the selection lambda
	// can capture a WeakPtr to it and call RequestDestroyWindow.
	// (DismissAllMenus only closes menus/dropdowns, not windows.)
	TSharedRef<SWindow> PickerWin = SNew(SWindow)
		.Title(FText::FromString(FString::Printf(TEXT("Select %s"), *ActivityLabel(ActivityType))))
		.ClientSize(FVector2D(500.f, 400.f))
		.SupportsMaximize(false)
		.SupportsMinimize(false);

	TWeakPtr<SWindow> WeakWin = PickerWin;

	auto CloseWithAsset = [this, WeakWin](const FAssetData& Asset)
	{
		SelectedAssetPath = Asset.GetSoftObjectPath().ToString();
		if (AssetNameLabel.IsValid())
			AssetNameLabel->SetText(FText::FromName(Asset.AssetName));
		if (TSharedPtr<SWindow> Win = WeakWin.Pin())
			FSlateApplication::Get().RequestDestroyWindow(Win.ToSharedRef());
	};

	FAssetPickerConfig Config;
	Config.Filter.ClassPaths.Add(FilterClass->GetClassPathName());

	// Anomaly: also surface game-native EotRAnomalyProfile assets alongside ForgeAnomalyProfile
	if (ActivityType == EForgeSidebarState::AnomalySetup)
	{
		Config.Filter.ClassPaths.Add(
			FTopLevelAssetPath(TEXT("/Script/EchoesOfTheRozlom"), TEXT("EotRAnomalyProfile")));
	}

	Config.bAllowNullSelection  = false;
	Config.bShowBottomToolbar   = true;
	Config.bAutohideSearchBar   = false;
	Config.InitialAssetViewType = EAssetViewType::List;
	Config.OnAssetSelected      = FOnAssetSelected::CreateLambda(CloseWithAsset);
	Config.OnAssetDoubleClicked = FOnAssetDoubleClicked::CreateLambda(CloseWithAsset);

	FContentBrowserModule& CBM =
		FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	PickerWin->SetContent(CBM.Get().CreateAssetPicker(Config));

	if (GEditor) GEditor->EditorAddModalWindow(PickerWin);
	return FReply::Handled();
}

// ─── Place ────────────────────────────────────────────────────────────────────

FReply SForgeActivityPanel::OnPlaceActor()
{
	if (!GEditor) return FReply::Handled();

	UWorld* World = GEditor->GetEditorWorldContext().World();
	if (!World) return FReply::Handled();

	const FVector  Loc  = ViewportCameraLocation() + FVector(150.f, 0.f, 0.f);
	const FRotator Rot  = FRotator::ZeroRotator;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* Placed = nullptr;

	switch (ActivityType)
	{
	case EForgeSidebarState::NPCSetup:
	{
		AForgeNPCActor* A = World->SpawnActor<AForgeNPCActor>(Loc, Rot, Params);
		if (A && !SelectedAssetPath.IsEmpty())
		{
			A->Profile = Cast<UForgeNPCProfile>(
				StaticLoadObject(UForgeNPCProfile::StaticClass(), nullptr, *SelectedAssetPath));
		}
		Placed = A;
		break;
	}
	case EForgeSidebarState::AnomalySetup:
	{
		AForgeAnomalyActor* A = World->SpawnActor<AForgeAnomalyActor>(Loc, Rot, Params);
		if (A)
		{
			A->RadiusOverride = RadiusOverride;
			if (!SelectedAssetPath.IsEmpty())
			{
				// Load the asset generically, then dispatch to the correct property.
				// ForgeAnomalyProfile → Profile  |  EotRAnomalyProfile → EotProfile
				if (UObject* Raw = StaticLoadObject(UObject::StaticClass(), nullptr, *SelectedAssetPath))
				{
					if (UForgeAnomalyProfile* Forge = Cast<UForgeAnomalyProfile>(Raw))
						A->Profile    = Forge;
					else if (UPrimaryDataAsset* Eot = Cast<UPrimaryDataAsset>(Raw))
						A->EotProfile = Eot;
				}
			}
		}
		Placed = A;
		break;
	}
	case EForgeSidebarState::EnemySetup:
	{
		AForgeEnemyActor* A = World->SpawnActor<AForgeEnemyActor>(Loc, Rot, Params);
		if (A)
		{
			A->SpawnCount = SpawnCount;
			if (!SelectedAssetPath.IsEmpty())
			{
				A->EnemyProfile = Cast<UForgeNPCProfile>(
					StaticLoadObject(UForgeNPCProfile::StaticClass(), nullptr, *SelectedAssetPath));
			}
		}
		Placed = A;
		break;
	}
	case EForgeSidebarState::PropsSetup:
	{
		AForgePropActor* A = World->SpawnActor<AForgePropActor>(Loc, Rot, Params);
		if (A) A->bIsInteractable = bIsInteractable;
		Placed = A;
		break;
	}
	default: break;
	}

	if (Placed)
	{
		GEditor->SelectNone(true, true);
		GEditor->SelectActor(Placed, true, true);
		GEditor->NoteSelectionChange();
		OnActorPlaced.ExecuteIfBound(Placed);
	}

	return FReply::Handled();
}

// ─── Helpers ─────────────────────────────────────────────────────────────────

FString SForgeActivityPanel::ActivityLabel(EForgeSidebarState Type)
{
	switch (Type)
	{
	case EForgeSidebarState::NPCSetup:     return TEXT("Add NPC");
	case EForgeSidebarState::AnomalySetup: return TEXT("Add Anomaly");
	case EForgeSidebarState::EnemySetup:   return TEXT("Add Enemy");
	case EForgeSidebarState::PropsSetup:   return TEXT("Add Prop");
	default:                               return TEXT("Configure");
	}
}

UClass* SForgeActivityPanel::ProfileClassFor(EForgeSidebarState Type)
{
	switch (Type)
	{
	case EForgeSidebarState::NPCSetup:     return UForgeNPCProfile::StaticClass();
	case EForgeSidebarState::AnomalySetup: return UForgeAnomalyProfile::StaticClass();
	case EForgeSidebarState::EnemySetup:   return UForgeNPCProfile::StaticClass();
	default:                               return nullptr;
	}
}

FVector SForgeActivityPanel::ViewportCameraLocation()
{
	if (GEditor)
	{
		if (FViewport* VP = GEditor->GetActiveViewport())
		{
			if (FEditorViewportClient* Client =
				static_cast<FEditorViewportClient*>(VP->GetClient()))
			{
				return Client->GetViewLocation();
			}
		}
	}
	return FVector::ZeroVector;
}

#undef LOCTEXT_NAMESPACE
