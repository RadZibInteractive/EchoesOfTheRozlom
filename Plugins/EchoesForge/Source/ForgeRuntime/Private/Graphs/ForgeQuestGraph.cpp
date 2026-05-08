// © 2026 RadZib. All rights reserved.

#include "Graphs/ForgeQuestGraph.h"
#include "Data/ForgeQuestData.h"

void UForgeQuestGraph::SetQuestData(UForgeQuestData* InData)
{
	QuestData = InData;
}

void UForgeQuestGraph::Initialize()
{
	if (!QuestData) return;

	ObjectiveProgress.Empty();
	for (const FForgeObjectiveData& Obj : QuestData->Objectives)
	{
		ObjectiveProgress.Add(Obj.ObjectiveId, 0);
	}
	CurrentState = EForgeQuestState::Inactive;
}

void UForgeQuestGraph::ActivateQuest()
{
	if (CurrentState != EForgeQuestState::Inactive) return;
	TransitionTo(EForgeQuestState::Active);
	TransitionTo(EForgeQuestState::InProgress);
}

void UForgeQuestGraph::FailQuest()
{
	if (CurrentState == EForgeQuestState::Completed) return;
	TransitionTo(EForgeQuestState::Failed);
}

void UForgeQuestGraph::ProgressObjective(FName ObjectiveId, int32 Amount)
{
	if (CurrentState != EForgeQuestState::InProgress) return;
	if (!ObjectiveProgress.Contains(ObjectiveId)) return;

	ObjectiveProgress[ObjectiveId] += Amount;
	OnObjectiveUpdated.Broadcast(ObjectiveId, ObjectiveProgress[ObjectiveId]);
	CheckCompletion();
}

bool UForgeQuestGraph::IsAllObjectivesComplete() const
{
	if (!QuestData) return false;

	for (const FForgeObjectiveData& Obj : QuestData->Objectives)
	{
		const int32* Progress = ObjectiveProgress.Find(Obj.ObjectiveId);
		if (!Progress || *Progress < Obj.RequiredCount)
		{
			return false;
		}
	}
	return true;
}

void UForgeQuestGraph::TransitionTo(EForgeQuestState NewState)
{
	CurrentState = NewState;
	OnQuestStateChanged.Broadcast(QuestData ? QuestData->QuestId : NAME_None, NewState);
}

void UForgeQuestGraph::CheckCompletion()
{
	if (IsAllObjectivesComplete())
	{
		TransitionTo(EForgeQuestState::Completed);
	}
}
