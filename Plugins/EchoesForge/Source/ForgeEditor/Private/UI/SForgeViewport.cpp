// © 2026 RadZib. All rights reserved.

#include "UI/SForgeViewport.h"
#include "Editor.h"

// ─── Viewport Client ─────────────────────────────────────────────────────────

FForgeViewportClient::FForgeViewportClient(const TWeakPtr<SEditorViewport>& InViewport)
	: FEditorViewportClient(nullptr, nullptr, InViewport)
{
	SetRealtime(true);
	bSetListenerPosition    = false;
	bAllowCinematicControl  = false;
	SetViewportType(LVT_Perspective);
	SetViewLocation(FVector(-500.f, 0.f, 300.f));
	SetViewRotation(FRotator(-25.f, 0.f, 0.f));
	SetCameraSpeedSetting(4);
	bDrawAxes = false;

	// Start in free-fly mode: RMB+drag = look, RMB+WASD = fly, scroll = zoom.
	// Alt+LMB orbit still works when held, but camera is never locked to a pivot.
	ToggleOrbitCamera(false);
}

UWorld* FForgeViewportClient::GetWorld() const
{
	if (GEditor)
	{
		if (UWorld* EditorWorld = GEditor->GetEditorWorldContext().World())
		{
			return EditorWorld;
		}
	}
	return FEditorViewportClient::GetWorld();
}

// ─── Slate Widget ─────────────────────────────────────────────────────────────

void SForgeViewport::Construct(const FArguments& InArgs)
{
	SEditorViewport::Construct(SEditorViewport::FArguments());
}

SForgeViewport::~SForgeViewport()
{
	if (ViewportClient.IsValid())
	{
		ViewportClient->Viewport = nullptr;
	}
}

TSharedRef<FEditorViewportClient> SForgeViewport::MakeEditorViewportClient()
{
	ViewportClient = MakeShared<FForgeViewportClient>(SharedThis(this));
	return ViewportClient.ToSharedRef();
}
