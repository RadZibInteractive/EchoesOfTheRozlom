// Minimal GameplayTagStack implementation to satisfy build & replication.
// If you later restore the original project version, you can replace this file.

#pragma once

#include "GameplayTagContainer.h"
#include "GameplayTagStack.generated.h"

/** One tag + its stack count */
USTRUCT(BlueprintType)
struct FGameplayTagStack
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag Tag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 StackCount = 0;
};

/**
 * Replicable container of tag stacks.
 * Kept simple: replicated as an array (not NetSerialize optimized).
 */
USTRUCT(BlueprintType)
struct FGameplayTagStackContainer
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FGameplayTagStack> Stacks;

	/** Adds StackCount stacks to Tag (ignored if StackCount <= 0) */
	void AddStack(const FGameplayTag& Tag, int32 StackCount)
	{
		if (!Tag.IsValid() || StackCount <= 0) return;

		for (FGameplayTagStack& S : Stacks)
		{
			if (S.Tag == Tag)
			{
				S.StackCount += StackCount;
				return;
			}
		}

		FGameplayTagStack NewStack;
		NewStack.Tag = Tag;
		NewStack.StackCount = StackCount;
		Stacks.Add(NewStack);
	}

	/** Removes StackCount stacks from Tag (ignored if StackCount <= 0) */
	void RemoveStack(const FGameplayTag& Tag, int32 StackCount)
	{
		if (!Tag.IsValid() || StackCount <= 0) return;

		for (int32 i = 0; i < Stacks.Num(); ++i)
		{
			if (Stacks[i].Tag == Tag)
			{
				Stacks[i].StackCount = FMath::Max(0, Stacks[i].StackCount - StackCount);
				if (Stacks[i].StackCount == 0)
				{
					Stacks.RemoveAtSwap(i);
				}
				return;
			}
		}
	}

	int32 GetStackCount(const FGameplayTag& Tag) const
	{
		if (!Tag.IsValid()) return 0;

		for (const FGameplayTagStack& S : Stacks)
		{
			if (S.Tag == Tag)
			{
				return S.StackCount;
			}
		}
		return 0;
	}

	bool HasTag(const FGameplayTag& Tag) const
	{
		return GetStackCount(Tag) > 0;
	}
};
