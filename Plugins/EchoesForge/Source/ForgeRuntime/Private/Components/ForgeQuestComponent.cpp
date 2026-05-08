// © 2026 RadZib. All rights reserved.

#include "Components/ForgeQuestComponent.h"
#include "Graphs/ForgeQuestGraph.h"
#include "Data/ForgeQuestData.h"

UForgeQuestComponent::UForgeQuestComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UForgeQuestComponent::AcceptQuest(UForgeQuestData* QuestData)
{
	if (!QuestData) return;
	if (ActiveGraphs.Contains(QuestData->QuestId)) return;

	UForgeQuestGraph* Graph = NewObject<UForgeQuestGraph>(this);
	Graph->SetQuestData(QuestData);
	Graph->Initialize();
	Graph->ActivateQuest();

	ActiveGraphs.Add(QuestData->QuestId, Graph);
}

void UForgeQuestComponent::ProgressObjective(FName QuestId, FName ObjectiveId, int32 Amount)
{
	if (UForgeQuestGraph* Graph = ActiveGraphs.FindRef(QuestId))
	{
		Graph->ProgressObjective(ObjectiveId, Amount);

		if (Graph->GetQuestState() == EForgeQuestState::Completed ||
			Graph->GetQuestState() == EForgeQuestState::Failed)
		{
			ActiveGraphs.Remove(QuestId);
		}
	}
}

EForgeQuestState UForgeQuestComponent::GetQuestState(FName QuestId) const
{
	if (const UForgeQuestGraph* Graph = ActiveGraphs.FindRef(QuestId))
	{
		return Graph->GetQuestState();
	}
	return EForgeQuestState::Inactive;
}

void UForgeQuestComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (auto& Pair : ActiveGraphs)
	{
		Pair.Value->Tick(DeltaTime);
	}
}
