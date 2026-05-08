// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

// ─── SForgePanel ──────────────────────────────────────────────────────────────
//
// Abstract base class for all Forge side-panel widgets.
//
// Implements the Template Method pattern:
//
//   Subclass::Construct()
//     └─ BuildLayout()         ← call this at the END of your Construct()
//          ├─ GetPanelTitle()  ← override: return the header text
//          ├─ GetTitleColor()  ← override (optional): accent color
//          ├─ BuildHeaderExtras() ← override (optional): buttons right of title
//          ├─ BuildContent()   ← MUST override: the main body
//          └─ BuildFooter()    ← override (optional): action row below content
//
// Benefits
//  • Every panel gets the same dark-bordered layout for free.
//  • Title, separator and footer are composed once, not copy-pasted.
//  • Call sites only implement what makes each panel unique.
//
// Example subclass:
//
//   class SForgeMyPanel : public SForgePanel
//   {
//   public:
//       SLATE_BEGIN_ARGS(SForgeMyPanel) {} SLATE_END_ARGS()
//       void Construct(const FArguments&)
//       {
//           /* ... store args ... */
//           BuildLayout(); // ← last line
//       }
//   protected:
//       virtual FText GetPanelTitle() const override
//           { return LOCTEXT("T","My Panel"); }
//       virtual TSharedRef<SWidget> BuildContent() override
//           { return SNew(STextBlock).Text(...); }
//   };

class FORGEEDITOR_API SForgePanel : public SCompoundWidget
{
protected:

	// ── Template-method hooks — override in subclasses ────────────────────────

	/** Text shown in the panel header. */
	virtual FText GetPanelTitle() const { return FText::GetEmpty(); }

	/** Accent color of the title text (default: brand cyan). */
	virtual FLinearColor GetTitleColor() const
	{
		return FLinearColor(0.45f, 0.82f, 0.92f);
	}

	/**
	 * Widgets placed to the RIGHT of the title label in the header row.
	 * Useful for compact action buttons (e.g., "👁 Visualize", "+ Add").
	 * Default: returns SNullWidget (no extras).
	 */
	virtual TSharedRef<SWidget> BuildHeaderExtras()
	{
		return SNullWidget::NullWidget;
	}

	/**
	 * The main body of the panel — subclasses MUST implement this.
	 * Placed between the separator and the footer.
	 */
	virtual TSharedRef<SWidget> BuildContent() = 0;

	/**
	 * Optional action row placed below the content area.
	 * Typical uses: "▶ Place in World", "💾 Save", "↺ Reset".
	 * Default: returns SNullWidget (no footer).
	 */
	virtual TSharedRef<SWidget> BuildFooter()
	{
		return SNullWidget::NullWidget;
	}

	// ── Assembly ──────────────────────────────────────────────────────────────

	/**
	 * Assemble the common layout and assign it to ChildSlot.
	 * Subclasses MUST call this at the very end of their Construct().
	 */
	void BuildLayout();
};
