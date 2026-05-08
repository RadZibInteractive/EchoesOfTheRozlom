// © 2026 RadZib. All rights reserved.

#include "Graphs/ForgeLogicGraph.h"

static const int32 MaxExecutionDepth = 64;

void UForgeLogicGraph::Initialize()
{
	RuntimeVariables.Empty();
}

void UForgeLogicGraph::TriggerEvent(FName EventNodeId)
{
	ExecuteNode(EventNodeId, 0);
}

void UForgeLogicGraph::SetVariable(FName VarName, const FString& Value)
{
	RuntimeVariables.Add(VarName, Value);
}

FString UForgeLogicGraph::GetVariable(FName VarName) const
{
	const FString* Val = RuntimeVariables.Find(VarName);
	return Val ? *Val : FString();
}

const TSet<FName>& UForgeLogicGraph::GetSafeActionNames()
{
	static TSet<FName> SafeSet =
	{
		FName("SpawnActor"),
		FName("DestroyActor"),
		FName("PlaySound"),
		FName("PlayVFX"),
		FName("SetVariable"),
		FName("SetQuestObjective"),
		FName("ShowMessage"),
		FName("OpenDoor"),
		FName("CloseDoor"),
		FName("EnableTrigger"),
		FName("DisableTrigger"),
		FName("TeleportPlayer"),
		FName("GiveItem"),
		FName("RemoveItem"),
	};
	return SafeSet;
}

void UForgeLogicGraph::ExecuteNode(FName NodeId, int32 Depth)
{
	// Guard against runaway graphs
	if (Depth >= MaxExecutionDepth) return;

	const FForgeLogicNode* Node = Nodes.FindByPredicate([&](const FForgeLogicNode& N){ return N.NodeId == NodeId; });
	if (!Node) return;

	SetActiveNode(NodeId);

	if (Node->NodeType == EForgeNodeType::Condition)
	{
		// Output[0] = true branch, Output[1] = false branch
		const bool bResult = EvaluateCondition(*Node);
		const int32 BranchIdx = bResult ? 0 : 1;
		if (Node->OutputNodeIds.IsValidIndex(BranchIdx))
		{
			ExecuteNode(Node->OutputNodeIds[BranchIdx], Depth + 1);
		}
		return;
	}

	if (Node->NodeType == EForgeNodeType::Action)
	{
		// Enforce safe node set before executing
		if (GetSafeActionNames().Contains(Node->ActionName))
		{
			BroadcastEvent(Node->ActionName);
		}
	}

	for (const FName& NextId : Node->OutputNodeIds)
	{
		ExecuteNode(NextId, Depth + 1);
	}
}

bool UForgeLogicGraph::EvaluateCondition(const FForgeLogicNode& Node) const
{
	const FString* LHS = Node.Parameters.Find(FName("Variable"));
	const FString* RHS = Node.Parameters.Find(FName("Value"));
	if (!LHS || !RHS) return false;

	const FString* Current = RuntimeVariables.Find(FName(**LHS));
	return Current && (*Current == *RHS);
}
