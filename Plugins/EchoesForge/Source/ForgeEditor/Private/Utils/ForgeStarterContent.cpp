// © 2026 RadZib. All rights reserved.

#include "Utils/ForgeStarterContent.h"

#include "Data/ForgeNPCProfile.h"
#include "Data/ForgeAnomalyProfile.h"
#include "Data/ForgeDialogueData.h"
#include "Data/ForgeQuestData.h"
#include "Data/ForgeTypes.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"
#include "Misc/PackageName.h"

#define LOCTEXT_NAMESPACE "ForgeStarterContent"

// ─── Statics ─────────────────────────────────────────────────────────────────

const FString FForgeStarterContent::RootPath = TEXT("/Game/ForgeContent/Starter");

// File-scope creation counter — reset at the start of each CreateAll() call.
// Not thread-safe, but editor-only so that's fine.
static int32 GForgeCreatedCount = 0;

// ─── Public entry ─────────────────────────────────────────────────────────────

int32 FForgeStarterContent::CreateAll()
{
	GForgeCreatedCount = 0;

	CreateNPCProfiles();
	CreateAnomalyProfiles();
	CreateDialogue();
	CreateQuest();

	if (GForgeCreatedCount > 0)
	{
		IAssetRegistry& AR = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();
		AR.ScanPathsSynchronous({ RootPath }, /*bForceRescan=*/true);
	}

	UE_LOG(LogTemp, Log, TEXT("ForgeStarterContent: created %d asset(s) under %s"),
		GForgeCreatedCount, *RootPath);
	return GForgeCreatedCount;
}

// ─── NPC Profiles ─────────────────────────────────────────────────────────────

void FForgeStarterContent::CreateNPCProfiles()
{
	// Guard
	if (!AssetExists(RootPath / TEXT("FP_Guard")))
	{
		if (UForgeNPCProfile* P = CreateAsset<UForgeNPCProfile>(TEXT("FP_Guard")))
		{
			P->ProfileId    = FName("FP_Guard");
			P->DisplayName  = LOCTEXT("GuardName",   "Zone Guard");
			P->Archetype    = EForgeNPCArchetype::Guard;
			P->AggroRules.SightRadius    = 1800.f;
			P->AggroRules.HearingRadius  = 900.f;
			P->AggroRules.bAttackOnSight = true;
			P->AggroRules.bCallForHelp   = true;
			SaveAndRegister(P, P->GetPackage());
		}
	}

	// Merchant
	if (!AssetExists(RootPath / TEXT("FP_Merchant")))
	{
		if (UForgeNPCProfile* P = CreateAsset<UForgeNPCProfile>(TEXT("FP_Merchant")))
		{
			P->ProfileId    = FName("FP_Merchant");
			P->DisplayName  = LOCTEXT("MerchantName", "Wandering Merchant");
			P->Archetype    = EForgeNPCArchetype::Merchant;
			P->AggroRules.bAttackOnSight = false;
			P->AggroRules.bCallForHelp   = false;
			P->AggroRules.SightRadius    = 600.f;
			SaveAndRegister(P, P->GetPackage());
		}
	}

	// Quest Giver
	if (!AssetExists(RootPath / TEXT("FP_QuestGiver")))
	{
		if (UForgeNPCProfile* P = CreateAsset<UForgeNPCProfile>(TEXT("FP_QuestGiver")))
		{
			P->ProfileId    = FName("FP_QuestGiver");
			P->DisplayName  = LOCTEXT("QuestGiverName", "Zone Elder");
			P->Archetype    = EForgeNPCArchetype::QuestGiver;
			P->AggroRules.bAttackOnSight = false;
			P->AggroRules.bCallForHelp   = false;
			P->AggroRules.SightRadius    = 400.f;
			SaveAndRegister(P, P->GetPackage());
		}
	}

	// Enemy
	if (!AssetExists(RootPath / TEXT("FP_Stalker")))
	{
		if (UForgeNPCProfile* P = CreateAsset<UForgeNPCProfile>(TEXT("FP_Stalker")))
		{
			P->ProfileId    = FName("FP_Stalker");
			P->DisplayName  = LOCTEXT("StalkerName", "Stalker");
			P->Archetype    = EForgeNPCArchetype::Enemy;
			P->AggroRules.SightRadius    = 2200.f;
			P->AggroRules.HearingRadius  = 1200.f;
			P->AggroRules.bAttackOnSight = true;
			P->AggroRules.bCallForHelp   = true;
			P->AggroRules.AlertCooldownSeconds = 8.f;

			// One combat scenario: rush when healthy
			FForgeCombatScenario Rush;
			Rush.ScenarioId            = FName("Rush");
			Rush.Description           = LOCTEXT("RushDesc", "Rush the target aggressively");
			Rush.HealthThresholdMin    = 0.5f;
			Rush.HealthThresholdMax    = 1.0f;
			P->CombatScenarios.Add(Rush);

			// Fall back: take cover when wounded
			FForgeCombatScenario Cover;
			Cover.ScenarioId           = FName("Cover");
			Cover.Description          = LOCTEXT("CoverDesc", "Take cover and call for help");
			Cover.HealthThresholdMin   = 0.f;
			Cover.HealthThresholdMax   = 0.5f;
			P->CombatScenarios.Add(Cover);

			SaveAndRegister(P, P->GetPackage());
		}
	}
}

// ─── Anomaly Profiles ─────────────────────────────────────────────────────────

void FForgeStarterContent::CreateAnomalyProfiles()
{
	// Gravitational
	if (!AssetExists(RootPath / TEXT("FAP_Gravitational")))
	{
		if (UForgeAnomalyProfile* P = CreateAsset<UForgeAnomalyProfile>(TEXT("FAP_Gravitational")))
		{
			P->ProfileId               = FName("FAP_Gravitational");
			P->DisplayName             = LOCTEXT("GravName",    "Gravity Trap");
			P->AnomalyType             = EForgeAnomalyType::Gravitational;
			P->Radius                  = 600.f;
			P->Falloff                 = 0.6f;
			P->DamagePerSecond         = 5.f;
			P->DamageType              = EForgeDamageType::Rozlom;
			P->bBlocksPlayerPath       = false;
			P->RozlomRechargeMultiplier = 1.5f;
			SaveAndRegister(P, P->GetPackage());
		}
	}

	// Electrical
	if (!AssetExists(RootPath / TEXT("FAP_Electrical")))
	{
		if (UForgeAnomalyProfile* P = CreateAsset<UForgeAnomalyProfile>(TEXT("FAP_Electrical")))
		{
			P->ProfileId               = FName("FAP_Electrical");
			P->DisplayName             = LOCTEXT("ElecName",    "Static Arc");
			P->AnomalyType             = EForgeAnomalyType::Electrical;
			P->Radius                  = 400.f;
			P->Falloff                 = 0.4f;
			P->DamagePerSecond         = 12.f;
			P->DamageType              = EForgeDamageType::Electric;
			P->bBlocksPlayerPath       = true;
			P->RozlomRechargeMultiplier = 2.0f;
			SaveAndRegister(P, P->GetPackage());
		}
	}

	// Thermal
	if (!AssetExists(RootPath / TEXT("FAP_Thermal")))
	{
		if (UForgeAnomalyProfile* P = CreateAsset<UForgeAnomalyProfile>(TEXT("FAP_Thermal")))
		{
			P->ProfileId               = FName("FAP_Thermal");
			P->DisplayName             = LOCTEXT("ThermalName", "Heat Bloom");
			P->AnomalyType             = EForgeAnomalyType::Thermal;
			P->Radius                  = 500.f;
			P->Falloff                 = 0.5f;
			P->DamagePerSecond         = 8.f;
			P->DamageType              = EForgeDamageType::Fire;
			P->bBlocksPlayerPath       = false;
			P->RozlomRechargeMultiplier = 1.2f;
			SaveAndRegister(P, P->GetPackage());
		}
	}

	// Chemical
	if (!AssetExists(RootPath / TEXT("FAP_Chemical")))
	{
		if (UForgeAnomalyProfile* P = CreateAsset<UForgeAnomalyProfile>(TEXT("FAP_Chemical")))
		{
			P->ProfileId               = FName("FAP_Chemical");
			P->DisplayName             = LOCTEXT("ChemName",    "Acid Cloud");
			P->AnomalyType             = EForgeAnomalyType::Chemical;
			P->Radius                  = 350.f;
			P->Falloff                 = 0.3f;
			P->DamagePerSecond         = 15.f;
			P->DamageType              = EForgeDamageType::Chemical;
			P->bBlocksPlayerPath       = true;
			P->RozlomRechargeMultiplier = 1.0f;
			SaveAndRegister(P, P->GetPackage());
		}
	}
}

// ─── Dialogue ─────────────────────────────────────────────────────────────────

void FForgeStarterContent::CreateDialogue()
{
	if (AssetExists(RootPath / TEXT("FD_GuardGreeting"))) return;

	UForgeDialogueData* D = CreateAsset<UForgeDialogueData>(TEXT("FD_GuardGreeting"));
	if (!D) return;

	D->DialogueId  = FName("FD_GuardGreeting");
	D->EntryNodeId = FName("Node_0");

	// Node 0: NPC opens
	{
		FForgeDialogueNode N;
		N.NodeId  = FName("Node_0");
		N.Text    = LOCTEXT("Node0Text", "Halt. This area is under zone authority watch. State your business.");
		N.Emotion = EForgeDialogueEmotion::Neutral;
		N.ResponseNodeIds.Add(FName("Node_1"));
		N.ResponseNodeIds.Add(FName("Node_2"));
		D->Nodes.Add(N);
	}

	// Node 1: Player asks about the area
	{
		FForgeDialogueNode N;
		N.NodeId  = FName("Node_1");
		N.Text    = LOCTEXT("Node1Text", "I'm exploring the zone. Any anomalies nearby I should know about?");
		N.Emotion = EForgeDialogueEmotion::Neutral;
		N.ResponseNodeIds.Add(FName("Node_3"));
		D->Nodes.Add(N);
	}

	// Node 2: Player ends conversation
	{
		FForgeDialogueNode N;
		N.NodeId  = FName("Node_2");
		N.Text    = LOCTEXT("Node2Text", "Just passing through. I'll be on my way.");
		N.Emotion = EForgeDialogueEmotion::Neutral;
		// No responses = end of conversation
		D->Nodes.Add(N);
	}

	// Node 3: Guard reply about anomalies
	{
		FForgeDialogueNode N;
		N.NodeId  = FName("Node_3");
		N.Text    = LOCTEXT("Node3Text", "Two active gravitational pockets to the east. Don't go near the collapsed structures — Rozlom energy readings are off the charts. Stay sharp.");
		N.Emotion = EForgeDialogueEmotion::Whisper;
		// No responses = end of conversation
		D->Nodes.Add(N);
	}

	SaveAndRegister(D, D->GetPackage());
}

// ─── Quest ────────────────────────────────────────────────────────────────────

void FForgeStarterContent::CreateQuest()
{
	if (AssetExists(RootPath / TEXT("FQ_StarterZone"))) return;

	UForgeQuestData* Q = CreateAsset<UForgeQuestData>(TEXT("FQ_StarterZone"));
	if (!Q) return;

	Q->QuestId    = FName("FQ_StarterZone");
	Q->QuestTitle = LOCTEXT("QuestTitle", "Into the Rozlom");
	Q->QuestDescription = LOCTEXT("QuestDesc",
		"Explore the starter zone, locate the anomaly clusters, "
		"and report back to the Zone Elder with your findings.");

	Q->EstimatedCompletionMinutes = 15.f;
	Q->MechanicsComplexity        = 1;
	Q->bIsSandbox                 = false;
	Q->RecommendedPrice           = 0.f;

	// Objectives
	{
		FForgeObjectiveData Obj;
		Obj.ObjectiveId   = FName("Obj_Explore");
		Obj.Description   = LOCTEXT("ObjExplore",   "Explore the zone perimeter");
		Obj.RequiredCount = 1;
		Q->Objectives.Add(Obj);
	}
	{
		FForgeObjectiveData Obj;
		Obj.ObjectiveId   = FName("Obj_Anomalies");
		Obj.Description   = LOCTEXT("ObjAnomalies", "Locate 3 active anomaly clusters");
		Obj.RequiredCount = 3;
		Q->Objectives.Add(Obj);
	}
	{
		FForgeObjectiveData Obj;
		Obj.ObjectiveId   = FName("Obj_Report");
		Obj.Description   = LOCTEXT("ObjReport",    "Report findings to the Zone Elder");
		Obj.RequiredCount = 1;
		Q->Objectives.Add(Obj);
	}

	// Reward
	Q->Reward.RZLMAmount = 250;

	SaveAndRegister(Q, Q->GetPackage());
}

// ─── Asset helpers ────────────────────────────────────────────────────────────

bool FForgeStarterContent::AssetExists(const FString& PackagePath)
{
	FString Filename;
	if (!FPackageName::TryConvertLongPackageNameToFilename(
		PackagePath, Filename, FPackageName::GetAssetPackageExtension()))
	{
		return false;
	}
	return FPaths::FileExists(Filename);
}

template<typename T>
T* FForgeStarterContent::CreateAsset(const FString& AssetName)
{
	const FString PackagePath = RootPath / AssetName;

	UPackage* Package = CreatePackage(*PackagePath);
	if (!Package)
	{
		UE_LOG(LogTemp, Warning, TEXT("ForgeStarterContent: failed to create package %s"), *PackagePath);
		return nullptr;
	}
	Package->FullyLoad();

	T* Asset = NewObject<T>(Package, *AssetName, RF_Public | RF_Standalone | RF_Transactional);
	if (!Asset)
	{
		UE_LOG(LogTemp, Warning, TEXT("ForgeStarterContent: failed to create asset %s"), *AssetName);
		return nullptr;
	}

	return Asset;
}

void FForgeStarterContent::SaveAndRegister(UObject* Asset, UPackage* Package)
{
	if (!Asset || !Package) return;

	FAssetRegistryModule::AssetCreated(Asset);
	Asset->MarkPackageDirty();

	FString PackageName = Package->GetName();
	FString Filename;
	if (FPackageName::TryConvertLongPackageNameToFilename(
		PackageName, Filename, FPackageName::GetAssetPackageExtension()))
	{
		FSavePackageArgs SaveArgs;
		SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
		SaveArgs.Error         = GWarn;
		UPackage::SavePackage(Package, Asset, *Filename, SaveArgs);
		++GForgeCreatedCount;
	}
}

#undef LOCTEXT_NAMESPACE

// ─── Template instantiations ──────────────────────────────────────────────────
template UForgeNPCProfile*     FForgeStarterContent::CreateAsset<UForgeNPCProfile>    (const FString&);
template UForgeAnomalyProfile* FForgeStarterContent::CreateAsset<UForgeAnomalyProfile>(const FString&);
template UForgeDialogueData*   FForgeStarterContent::CreateAsset<UForgeDialogueData>  (const FString&);
template UForgeQuestData*      FForgeStarterContent::CreateAsset<UForgeQuestData>     (const FString&);
