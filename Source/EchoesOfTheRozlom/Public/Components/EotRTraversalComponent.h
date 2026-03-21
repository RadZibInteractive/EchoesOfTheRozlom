// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/Structs/EotRTraversalCheckResult.h"
#include "MotionWarpingComponent.h"

#include "EotRTraversalComponent.generated.h"

UCLASS(Abstract, Blueprintable)
class ECHOESOFTHEROZLOM_API UEotRTraversalComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, Category = "EotR|Traversal")
    bool TryFindTraversal(FEotRTraversalCheckResult& OutResult);

    UFUNCTION(BlueprintNativeEvent, Category = "EotR|Traversal")
    void UpdateWarpTargets(const FEotRTraversalCheckResult& TraversalResult);

    void SetMotionWarpingComponent(UMotionWarpingComponent* InWarpComp) { MotionWarpingComponent = InWarpComp; }

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default")
    TObjectPtr<UMotionWarpingComponent> MotionWarpingComponent = nullptr;

public:
    UPROPERTY(Transient)
    FEotRTraversalCheckResult CachedResult;
};