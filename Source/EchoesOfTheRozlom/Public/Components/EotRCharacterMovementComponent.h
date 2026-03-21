#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayTagContainer.h"

#include "Data/Enums/EotRMovementGait.h"
#include "Data/Structs/EotRLandingInfo.h"

class UCurveFloat;
struct FTimerHandle;

#include "EotRCharacterMovementComponent.generated.h"

UCLASS()
class ECHOESOFTHEROZLOM_API UEotRCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	// Audio
	UFUNCTION(BlueprintCallable, Category = "EotR|Movement")
	void PlayFoley(const FGameplayTag& Tag, float VolumeMultiplier);

	// Movement
	void SetGaitSpeedMap(const TMap<EEotRMovementGait, FVector>& NewGaitSpeedMap) { GaitSpeedMap = NewGaitSpeedMap; }
	EEotRMovementGait GetCurrentMovementGait() const { return MovementGait; }
	FEotRLandingInfo GetCurrentLandingInfo() const { return LandingInfo; }

protected:
	// UActorComponent
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void InitializeComponent() override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

private:
	// Movement
	void UpdateMovement(float DeltaTime);
	void ClearJustLanded();
	bool CanSprint() const;

private:
	// Movement
	UPROPERTY(Transient)
	TObjectPtr<UCurveFloat> StrafeSpeedMapCurve = nullptr;

	TMap<EEotRMovementGait, FVector> GaitSpeedMap;
	EEotRMovementGait MovementGait = EEotRMovementGait::Walk;
	FEotRLandingInfo LandingInfo;
	FTimerHandle JustLandedTimerHandle;
};