// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "SEditorViewport.h"
#include "EditorViewportClient.h"

class FForgeViewportClient : public FEditorViewportClient
{
public:
	explicit FForgeViewportClient(const TWeakPtr<SEditorViewport>& InViewport);
	virtual ~FForgeViewportClient() override = default;

	virtual UWorld* GetWorld() const override;
	// Free-fly navigation: RMB+drag = look, RMB+WASD = fly, scroll = zoom.
	// Alt+LMB still orbits when explicitly held — but camera is not locked to any pivot.
	virtual bool ShouldOrbitCamera() const override { return false; }
};

class SForgeViewport : public SEditorViewport
{
public:
	SLATE_BEGIN_ARGS(SForgeViewport) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	virtual ~SForgeViewport() override;

protected:
	virtual TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;

private:
	TSharedPtr<FForgeViewportClient>  ViewportClient;
};
