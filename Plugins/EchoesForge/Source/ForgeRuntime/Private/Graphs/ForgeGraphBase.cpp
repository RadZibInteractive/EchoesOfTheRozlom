// © 2026 RadZib. All rights reserved.

#include "Graphs/ForgeGraphBase.h"

void UForgeGraphBase::SetActiveNode(FName NodeId)
{
	ActiveNodeId = NodeId;
}

void UForgeGraphBase::BroadcastEvent(FName EventId)
{
	OnGraphEvent.Broadcast(EventId);
}
