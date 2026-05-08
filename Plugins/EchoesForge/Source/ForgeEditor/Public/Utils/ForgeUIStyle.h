// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateTypes.h"
#include "Styling/AppStyle.h"

// ─── ForgeButtonStyle ─────────────────────────────────────────────────────────
// Returns a persistent FButtonStyle keyed by Normal+Hover color pair.
// Styles are heap-allocated so references never get invalidated by TMap rehash.
// Usage:
//   SNew(SButton).ButtonStyle(&ForgeButtonStyle(NormalColor, HoverColor))
// ─────────────────────────────────────────────────────────────────────────────

inline const FButtonStyle& ForgeButtonStyle(FLinearColor Normal, FLinearColor Hover)
{
	// Pack a linear color to 24-bit (8-bit/channel, sufficient for editor UI)
	auto Pack = [](FLinearColor C) -> uint32
	{
		return (uint32)(FMath::Clamp(C.R, 0.f, 1.f) * 255.f)
			| ((uint32)(FMath::Clamp(C.G, 0.f, 1.f) * 255.f) << 8)
			| ((uint32)(FMath::Clamp(C.B, 0.f, 1.f) * 255.f) << 16);
	};

	const uint64 Key = (uint64)Pack(Normal) | ((uint64)Pack(Hover) << 32);

	// Styles stored as heap-allocated objects — references stay valid across rehash
	static TMap<uint64, TUniquePtr<FButtonStyle>> Cache;

	if (TUniquePtr<FButtonStyle>* Hit = Cache.Find(Key))
	{
		return **Hit;
	}

	// Use WhiteBrush (a 1×1 white texture always present in AppStyle)
	// and tint it per-state — this gives clean flat colored buttons.
	const FSlateBrush* White = FAppStyle::Get().GetBrush("WhiteBrush");

	auto MakeBrush = [&](FLinearColor Tint) -> FSlateBrush
	{
		FSlateBrush B = *White;
		B.TintColor   = FSlateColor(Tint);
		return B;
	};

	auto Style = MakeUnique<FButtonStyle>();
	Style->SetNormal (MakeBrush(Normal));
	Style->SetHovered(MakeBrush(Hover));
	Style->SetPressed(MakeBrush(Normal * 0.72f));
	Style->SetDisabled(MakeBrush(Normal * 0.40f));
	Style->NormalPadding  = FMargin(0.f);
	Style->PressedPadding = FMargin(0.f);

	Cache.Add(Key, MoveTemp(Style));
	return *Cache[Key];
}

// ─── Convenience presets ──────────────────────────────────────────────────────
//
// Brand palette:  Cyan (primary) · Magenta (anomaly/danger) · Gold (confirm/patrol)
//
// Cyan  normal=(0.04,0.20,0.30)  hover=(0.06,0.36,0.52)
// Mag   normal=(0.22,0.04,0.20)  hover=(0.38,0.07,0.35)
// Gold  normal=(0.24,0.16,0.02)  hover=(0.40,0.28,0.04)

// Subtle transparent — reveals a cyan tint on hover
inline const FButtonStyle& ForgeSubtleBtn()
{
	return ForgeButtonStyle(
		FLinearColor(0.f,    0.f,    0.f,    0.f),
		FLinearColor(0.08f,  0.18f,  0.28f));
}

// CYAN — primary action (save, browse, inspect)
inline const FButtonStyle& ForgePrimaryBtn()
{
	return ForgeButtonStyle(
		FLinearColor(0.04f,  0.20f,  0.30f),
		FLinearColor(0.06f,  0.36f,  0.52f));
}

// GOLD — confirm / place in world / success
inline const FButtonStyle& ForgeConfirmBtn()
{
	return ForgeButtonStyle(
		FLinearColor(0.24f,  0.16f,  0.02f),
		FLinearColor(0.40f,  0.28f,  0.04f));
}

// DANGER — delete / destructive (red-magenta)
inline const FButtonStyle& ForgeDangerBtn()
{
	return ForgeButtonStyle(
		FLinearColor(0.32f,  0.05f,  0.10f),
		FLinearColor(0.50f,  0.08f,  0.16f));
}

// Deep CYAN — secondary navigation, response rows
inline const FButtonStyle& ForgeNavyBtn()
{
	return ForgeButtonStyle(
		FLinearColor(0.06f,  0.12f,  0.22f),
		FLinearColor(0.10f,  0.22f,  0.38f));
}

// MAGENTA — anomaly / special ability
inline const FButtonStyle& ForgeVioletBtn()
{
	return ForgeButtonStyle(
		FLinearColor(0.22f,  0.04f,  0.20f),
		FLinearColor(0.38f,  0.07f,  0.35f));
}

// GOLD bright — patrol / visualize
inline const FButtonStyle& ForgeAmberBtn()
{
	return ForgeButtonStyle(
		FLinearColor(0.28f,  0.18f,  0.02f),
		FLinearColor(0.44f,  0.30f,  0.04f));
}

// Ghost — dark panel background, cyan hover
inline const FButtonStyle& ForgeGhostBtn()
{
	return ForgeButtonStyle(
		FLinearColor(0.07f,  0.09f,  0.15f),
		FLinearColor(0.10f,  0.20f,  0.32f));
}
