// © 2026 RadZib. All rights reserved.

#include "UI/DAForgeThumbnailRenderer.h"

#include "CanvasItem.h"
#include "CanvasTypes.h"
#include "ForgeDataAsset.h"

bool UDAForgeThumbnailRenderer::CanVisualizeAsset(UObject* Object)
{
	if (UForgeDataAsset* ItemObj = Cast<UForgeDataAsset>(Object))
	{
		return !ItemObj->DisplayIcon.IsNull() || !ItemObj->DisplayTitle.IsEmpty();
	}
	return false;
}

void UDAForgeThumbnailRenderer::GetThumbnailSize(UObject* Object, float Zoom, uint32& OutWidth, uint32& OutHeight) const
{
	OutWidth = 256;
	OutHeight = 256;
}

void UDAForgeThumbnailRenderer::Draw(UObject* Object, int32 X, int32 Y, uint32 Width, uint32 Height,
                                           FRenderTarget* Viewport, FCanvas* Canvas, bool bAdditionalViewFamily)
{
    if (UForgeDataAsset* item = Cast<UForgeDataAsset>(Object)) {
        if (!item->DisplayIcon.IsNull()) {
            if (UTexture2D* icon = item->DisplayIcon.LoadSynchronous()) {
                FCanvasTileItem CanvasTile(FVector2D(X, Y), icon->GetResource(), FVector2D(Width, Height),
                                           FLinearColor::White);
                CanvasTile.BlendMode = SE_BLEND_Masked;
                CanvasTile.Draw(Canvas);
            }
        }
        
        if (!item->DisplayTitle.IsEmpty()) {
            const FString tittle = item->DisplayTitle;
            auto vtChars = UTF8_TO_TCHAR(*tittle);

            UFont* font = GEngine->GetMediumFont();
            int32 vtWidth = 0;
            int32 vtHeight = 0;
        	

            StringSize(font, vtWidth, vtHeight, vtChars);
            float padding = Height / 128.0f;
            float scaleX = Width / 64.0f;
            float scaleY = Height / 64.0f;

            FCanvasTextItem textItem(FVector2D(0.f, Height - padding - vtHeight * scaleY), FText::FromString(tittle),
                                     GEngine->GetMediumFont(), FLinearColor::White);

            textItem.EnableShadow(FLinearColor::Black);
            textItem.Scale = FVector2D(scaleX, scaleY);
            textItem.Draw(Canvas);
        }
    }
}

bool UDAForgeThumbnailRenderer::AllowsRealtimeThumbnails(UObject* Object) const
{
	return true;
}
