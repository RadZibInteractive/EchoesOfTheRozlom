// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ForgeDialogueData.generated.h"

UENUM(BlueprintType)
enum class EForgeDialogueEmotion : uint8
{
	Neutral,
	Whisper,
	Shout,
	Sarcastic,
	Fearful,
	Joyful
};

USTRUCT(BlueprintType)
struct FORGECORE_API FForgeDialogueCheck
{
	GENERATED_BODY()

	// Gameplay tag condition that must be satisfied to show this node
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName ConditionId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bNegate = false;
};

USTRUCT(BlueprintType)
struct FORGECORE_API FForgeDialogueNode
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName NodeId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (MultiLine = true))
	FText Text;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EForgeDialogueEmotion Emotion = EForgeDialogueEmotion::Neutral;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<USoundBase> VoiceAudio;

	// Body/face animation montage reference
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<class UAnimMontage> FacialAnimation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FForgeDialogueCheck> Conditions;

	// IDs of child nodes (player response options)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FName> ResponseNodeIds;
};

UCLASS(BlueprintType)
class FORGECORE_API UForgeDialogueData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Dialogue")
	FName DialogueId;

	UPROPERTY(EditDefaultsOnly, Category = "Dialogue")
	FName EntryNodeId;

	UPROPERTY(EditDefaultsOnly, Category = "Dialogue")
	TArray<FForgeDialogueNode> Nodes;

	const FForgeDialogueNode* FindNode(FName NodeId) const
	{
		return Nodes.FindByPredicate([&](const FForgeDialogueNode& N){ return N.NodeId == NodeId; });
	}

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("ForgeDialogue", GetFName());
	}
};
