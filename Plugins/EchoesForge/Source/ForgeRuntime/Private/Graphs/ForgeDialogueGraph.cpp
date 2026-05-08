// © 2026 RadZib. All rights reserved.

#include "Graphs/ForgeDialogueGraph.h"

void UForgeDialogueGraph::Initialize()
{
	bIsActive = false;
	DialogueData = nullptr;
}

void UForgeDialogueGraph::StartDialogue(UForgeDialogueData* InData)
{
	if (!InData) return;

	DialogueData = InData;
	bIsActive = true;
	AdvanceTo(InData->EntryNodeId);
}

void UForgeDialogueGraph::SelectResponse(FName ResponseNodeId)
{
	if (!bIsActive) return;
	AdvanceTo(ResponseNodeId);
}

TArray<FForgeDialogueNode> UForgeDialogueGraph::GetAvailableResponses() const
{
	TArray<FForgeDialogueNode> Available;
	if (!DialogueData || !bIsActive) return Available;

	const FForgeDialogueNode* Current = DialogueData->FindNode(ActiveNodeId);
	if (!Current) return Available;

	for (const FName& ResponseId : Current->ResponseNodeIds)
	{
		const FForgeDialogueNode* Response = DialogueData->FindNode(ResponseId);
		if (Response && EvaluateConditions(Response->Conditions))
		{
			Available.Add(*Response);
		}
	}
	return Available;
}

void UForgeDialogueGraph::AdvanceTo(FName NodeId)
{
	if (!DialogueData) return;

	const FForgeDialogueNode* Node = DialogueData->FindNode(NodeId);
	if (!Node)
	{
		bIsActive = false;
		OnDialogueFinished.Broadcast();
		return;
	}

	SetActiveNode(NodeId);
	OnNodeReached.Broadcast(*Node);

	// Auto-finish if no responses
	if (Node->ResponseNodeIds.IsEmpty())
	{
		bIsActive = false;
		OnDialogueFinished.Broadcast();
	}
}

bool UForgeDialogueGraph::EvaluateConditions(const TArray<FForgeDialogueCheck>& Conditions) const
{
	// Placeholder: all conditions pass until a gameplay tag query system is wired in
	return true;
}
