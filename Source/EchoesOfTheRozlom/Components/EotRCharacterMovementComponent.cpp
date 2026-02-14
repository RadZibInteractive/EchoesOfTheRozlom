#include "Components/EotRCharacterMovementComponent.h"

#include "FrameworkBase/EotRDeveloperSettings.h"
#include "EotRGameplayTags.h"

#include "GameFramework/Pawn.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "Curves/CurveFloat.h"
#include "KismetAnimationLibrary.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Data/Enums/EotRMovementGait.h"
#include "Data/Structs/EotRLandingInfo.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"

#include "GameplayTagContainer.h"
#include "Engine/GameInstance.h"
#include "FrameworkBase/EotRGameInstanceSubsystem.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"

void UEotRCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	UpdateMovement(DeltaTime);

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UEotRCharacterMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (const UEotRDeveloperSettings* Settings = GetDefault<UEotRDeveloperSettings>())
	{
		StrafeSpeedMapCurve = Settings->DefaultStrafeSpeedMapCurve.LoadSynchronous();
	}
}

void UEotRCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

	if (MovementMode == MOVE_Falling && PreviousMovementMode != MOVE_Falling)
	{
		const float SpeedXY = FVector(Velocity.X, Velocity.Y, 0.f).Size();

		const float VolumeMultiplier = FMath::GetMappedRangeValueClamped(
			FVector2D(0.f, 500.f),
			FVector2D(0.5f, 1.f),
			SpeedXY
		);

		PlayFoley(EotRTags::Foley_Event_Jump, VolumeMultiplier);
	}

	if (MovementMode == MOVE_Walking && PreviousMovementMode == MOVE_Falling)
	{
		LandingInfo.bJustLanded = true;
		LandingInfo.LandVelocity = Velocity;

		const float VolumeMultiplier = FMath::GetMappedRangeValueClamped(
			FVector2D(-500.f, -900.f),
			FVector2D(0.5f, 1.5f),
			Velocity.Z
		);

		PlayFoley(EotRTags::Foley_Event_Land, VolumeMultiplier);

		if (UWorld* World = GetWorld())
		{	
			World->GetTimerManager().SetTimer(
				JustLandedTimerHandle,
				this,
				&UEotRCharacterMovementComponent::ClearJustLanded,
				0.3f,
				false
			);
		}
	}
}

void UEotRCharacterMovementComponent::PlayFoley(const FGameplayTag& Tag, float VolumeMultiplier)
{
	if (!CharacterOwner)
	{
		return;
	}

	FGameplayTagContainer GroundTags;
	GroundTags.AddTag(EotRTags::Foley_Event_Run);
	GroundTags.AddTag(EotRTags::Foley_Event_RunBackwds);
	GroundTags.AddTag(EotRTags::Foley_Event_RunStrafe);
	GroundTags.AddTag(EotRTags::Foley_Event_Scuff);
	GroundTags.AddTag(EotRTags::Foley_Event_ScuffPivot);
	GroundTags.AddTag(EotRTags::Foley_Event_Walk);
	GroundTags.AddTag(EotRTags::Foley_Event_WalkBackwds);

	const bool bIsGroundTag = GroundTags.HasTag(Tag);

	bool bIsTraversing = false;

	if (IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(CharacterOwner))
	{
		if (UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent())
		{
			if (ASC->HasMatchingGameplayTag(EotRTags::State_Movement_Traversing))
			{
				bIsTraversing = true;
			}
		}
	}

	if (bIsGroundTag && !IsMovingOnGround() && !bIsTraversing)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World) return;

	const UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance) return;

	const UEotRGameInstanceSubsystem* GameInstanceSubsystem = GameInstance->GetSubsystem<UEotRGameInstanceSubsystem>();
	if (!GameInstanceSubsystem) return;

	USoundBase* Sound = GameInstanceSubsystem->GetSoundByTag(Tag);
	if (!Sound) return;

	UGameplayStatics::SpawnSoundAtLocation(
		this,
		Sound,
		CharacterOwner->GetMesh()->GetComponentLocation(),
		FRotator::ZeroRotator,
		VolumeMultiplier
	);
}

void UEotRCharacterMovementComponent::ClearJustLanded()
{
	LandingInfo.bJustLanded = false;
}

void UEotRCharacterMovementComponent::UpdateMovement(float DeltaTime)
{
	if (!PawnOwner)
	{
		return;
	}

	// Determine and save movement mode based on GAS state tags (Walk/Run/Sprint)
	if (IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(PawnOwner))
	{
		if (UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent())
		{
			if (ASC->HasMatchingGameplayTag(EotRTags::State_Movement_Crouching))
			{
				MovementGait = EEotRMovementGait::Crouch;
			}
			else if (ASC->HasMatchingGameplayTag(EotRTags::State_Movement_Sprinting))
			{
				if (CanSprint())
				{
					MovementGait = EEotRMovementGait::Sprint;
				}
				else {
					FGameplayTagContainer TagContainer;
					TagContainer.AddTag(EotRTags::Ability_Movement_Sprint);

					ASC->CancelAbilities(&TagContainer);

					if (ASC->HasMatchingGameplayTag(EotRTags::State_Movement_Running))
					{
						MovementGait = EEotRMovementGait::Run;
					}
					else
					{
						MovementGait = EEotRMovementGait::Walk;
					}
				}
			}
			else if (ASC->HasMatchingGameplayTag(EotRTags::State_Movement_Running))
			{
				MovementGait = EEotRMovementGait::Run;
			}
			else
			{
				MovementGait = EEotRMovementGait::Walk;
			}

		}
	}

	const float SpeedXY = FVector(Velocity.X, Velocity.Y, 0.f).Size();

	// Calculate max acceleration based on movement gait and current speed
	const float MappedAcceleration = FMath::GetMappedRangeValueClamped(
		FVector2D(300.f, 700.f),
		FVector2D(800.f, 300.f),
		SpeedXY
	);
	MaxAcceleration = (MovementGait == EEotRMovementGait::Sprint) ? MappedAcceleration : 800.f;

	// Adjust braking deceleration based on whether there is movement input
	const bool HasMovementInput = !GetPendingInputVector().IsNearlyZero();
	BrakingDecelerationWalking = HasMovementInput ? 500.f : 2000.f;

	// Calculate ground friction based on movement gait and current speed
	const float MappedGroundFriction = FMath::GetMappedRangeValueClamped(
		FVector2D(0.f, 500.f),
		FVector2D(5.f, 3.f),
		SpeedXY
	);
	GroundFriction = (MovementGait == EEotRMovementGait::Sprint) ? MappedGroundFriction : 5.f;

	// Calculate max speed based on movement gait and current speed
	const float Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, PawnOwner->GetControlRotation());
	const float AbsDirection = FMath::Abs(Direction);

	float CurveStrafeMap = 0.f;
	if (StrafeSpeedMapCurve)
	{
		CurveStrafeMap = StrafeSpeedMapCurve->GetFloatValue(AbsDirection);
	}
	const float StrafeSpeedMap = bOrientRotationToMovement ? 0.f : CurveStrafeMap;

	FVector GaitSpeed = FVector::ZeroVector;
	if (const FVector* Found = GaitSpeedMap.Find(MovementGait))
	{
		GaitSpeed = *Found;
	}

	const float StrafeSpeed_0_1 = FMath::GetMappedRangeValueClamped(
		FVector2D(0.f, 1.f),
		FVector2D(GaitSpeed.X, GaitSpeed.Y),
		StrafeSpeedMap
	);

	const float StrafeSpeed_1_2 = FMath::GetMappedRangeValueClamped(
		FVector2D(1.f, 2.f),
		FVector2D(GaitSpeed.Y, GaitSpeed.Z),
		StrafeSpeedMap
	);

	const float MaxSpeed = (StrafeSpeedMap < 1.f) ? StrafeSpeed_0_1 : StrafeSpeed_1_2;

	(MovementGait == EEotRMovementGait::Crouch ? MaxWalkSpeedCrouched : MaxWalkSpeed) = MaxSpeed;
}

bool UEotRCharacterMovementComponent::CanSprint() const
{
	const bool bLocal = PawnOwner->IsLocallyControlled();
	const FVector MoveVector = bLocal
		? GetPendingInputVector()
		: GetCurrentAcceleration();
	if (MoveVector.IsNearlyZero())
	{
		return false;
	}

	const FRotator ControlRotation = PawnOwner->GetControlRotation();
	const FRotator MoveRotation = MoveVector.ToOrientationRotator();
	const float YawDelta = FRotator::NormalizeAxis(MoveRotation.Yaw - ControlRotation.Yaw);
	const float AbsYawDelta = FMath::Abs(YawDelta);

	return bOrientRotationToMovement ? true : (AbsYawDelta < 60.f);
}