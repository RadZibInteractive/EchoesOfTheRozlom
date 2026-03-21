// © 2026 RadZib. All rights reserved.

#include "Characters/EotRHumanCharacter.h"

#include "FrameworkBase/EotRLocalPlayerSubsystem.h"
#include "Data/DataAssets/EotRInputTagConfig.h"

#include "Components/EotRInteractionComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/EotRCameraComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "FrameworkBase/EotRGameplayTags.h"
#include "GameplayTagContainer.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"

#include "AbilitySystem/EotRAbilitySystemComponent.h"

AEotRHumanCharacter::AEotRHumanCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	InteractionComponent = CreateDefaultSubobject<UEotRInteractionComponent>(TEXT("InteractionComponent"));

	USkeletalMeshComponent* LowerMesh = GetMesh();

	UpperMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("UpperMesh"));
	UpperMesh->SetupAttachment(LowerMesh);
	UpperMesh->AddTickPrerequisiteComponent(LowerMesh);

	LowerMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LowerMesh->SetVisibility(false);
	LowerMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;

	UpperMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	UpperMesh->SetVisibility(false);
	UpperMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;

	ViewHeadMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ViewHeadMesh"));
	ViewHeadMesh->SetupAttachment(UpperMesh);
	ViewHeadMesh->AddTickPrerequisiteComponent(UpperMesh);
	ViewHeadMesh->SetCollisionProfileName(TEXT("CharacterMesh"));
	ViewHeadMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ViewHeadMesh->SetOwnerNoSee(true);
	ViewHeadMesh->bCastHiddenShadow = true;

	FirstPersonCamera = CreateDefaultSubobject<UEotRCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(ViewHeadMesh);
	FirstPersonCamera->bUsePawnControlRotation = true;
}

void AEotRHumanCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	const UEotRHumanDataAsset* HumanData = Cast<UEotRHumanDataAsset>(CharacterData);

	if (!HumanData)
	{
		return;
	}

	if (HumanData->UpperMeshAsset)
	{
		UpperMesh->SetSkeletalMesh(HumanData->UpperMeshAsset);
	}

	if (HumanData->UpperAnimInstanceClass)
	{
		UpperMesh->SetAnimInstanceClass(HumanData->UpperAnimInstanceClass);
	}

	if (HumanData->ViewHeadMeshAsset)
	{
		ViewHeadMesh->SetSkeletalMesh(HumanData->ViewHeadMeshAsset);
	}

	if (HumanData->ViewAnimInstanceClass)
	{
		ViewHeadMesh->SetAnimInstanceClass(HumanData->ViewAnimInstanceClass);
	}

	for (int32 i = 0; i < HumanData->ViewPartMeshesAssets.Num(); ++i)
	{
		const FName PartName = *FString::Printf(TEXT("ViewPartMesh_%d"), i);
		USkeletalMeshComponent* NewComp =
			NewObject<USkeletalMeshComponent>(this, USkeletalMeshComponent::StaticClass(), PartName);

		NewComp->RegisterComponent();
		NewComp->AttachToComponent(ViewHeadMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		NewComp->SetSkeletalMesh(HumanData->ViewPartMeshesAssets[i]);
		NewComp->SetLeaderPoseComponent(ViewHeadMesh);
		NewComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		ViewPartMeshes.Add(NewComp);
	}

	FirstPersonCameraSocket = HumanData->FirstPersonCameraSocketName;

	FirstPersonCamera->AttachToComponent(
		ViewHeadMesh,
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		FirstPersonCameraSocket
	);
	FirstPersonCamera->SetRelativeLocation(FVector(-4.935916f, 7.721559f, 0.178092f));
	FirstPersonCamera->SetRelativeRotation(FRotator(-90.000098f, 1.321171f, 90.001102f));
	FirstPersonCamera->SetRelativeScale3D(FVector(0.55f, 0.55f, 0.55f));
}

void AEotRHumanCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (!IsLocallyControlled())
	{
		return;
	}

	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EnhancedInput) return;

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController) return;

	ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	if (!LocalPlayer) return;

	const UEotRLocalPlayerSubsystem* LocalPlayerSubsystem = LocalPlayer->GetSubsystem<UEotRLocalPlayerSubsystem>();
	if (!LocalPlayerSubsystem) return;

	const UEotRInputTagConfig* InputTagConfig = LocalPlayerSubsystem->GetInputTagConfig();
	if (!InputTagConfig) return;

	EnhancedInput->ClearActionBindings();

	for (const FEotRTaggedInputAction& Entry : InputTagConfig->TaggedInputActions)
	{
		if (!Entry.InputAction || !Entry.InputTag.IsValid())
		{
			continue;
		}
		
		EnhancedInput->BindAction(Entry.InputAction, ETriggerEvent::Triggered, this, &AEotRHumanCharacter::OnInputTagTriggered, Entry.InputTag);
	}
}

void AEotRHumanCharacter::SetAbilityAnimTarget(bool bUseAlternative)
{
	if (FGameplayAbilityActorInfo* ActorInfo = AbilitySystemComponent->AbilityActorInfo.Get())
	{
		if (bUseAlternative)
		{
			ActorInfo->SkeletalMeshComponent = UpperMesh;
		}
		else
		{
			ActorInfo->SkeletalMeshComponent = GetMesh();
		}
	}
}

void AEotRHumanCharacter::OnInputTagTriggered(const FInputActionValue& ActionValue, FGameplayTag InputTag)
{
	if (InputTag == EotRGameplayTags::Ability_Movement_Move)
	{
		const FVector2D MoveValue = ActionValue.Get<FVector2D>();

		AddMovementInput(GetActorRightVector(), CalculateMoveInput(true, MoveValue.X));
		AddMovementInput(GetActorForwardVector(), CalculateMoveInput(false, MoveValue.Y));

		return;
	}
	else if (InputTag == EotRGameplayTags::Ability_Camera_Look)
	{
		const FVector2D LookValue = ActionValue.Get<FVector2D>();

		AddControllerYawInput(CalculateCameraInput(true, LookValue.X));
		AddControllerPitchInput(CalculateCameraInput(false, LookValue.Y));

		return;
	}

	else
	{
		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
		{
			if (ActionValue.GetValueType() == EInputActionValueType::Boolean)
			{
				const bool bPressed = ActionValue.Get<bool>();

				if (bPressed)
				{
					ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(InputTag));
				}
				else
				{
					FGameplayTagContainer TagContainer;
					TagContainer.AddTag(InputTag);

					ASC->CancelAbilities(&TagContainer);
				}
			}
			else
			{
				ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(InputTag));
			}
		}
	}
}

float AEotRHumanCharacter::CalculateMoveInput(bool bIsRightAxis, float ActionValue, ECollisionChannel TraceChannel) const
{
	const UCapsuleComponent* Capsule = GetCapsuleComponent();
	const UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	const UWorld* World = GetWorld();

	if (ActionValue == 0.f)
	{
		return 0.0f;
	}
	if (MoveComp->MovementMode != MOVE_Walking)
	{
		return ActionValue;
	}

	FVector Dir = bIsRightAxis ? GetActorRightVector() : GetActorForwardVector();
	if (ActionValue < 0.f)
	{
		Dir = -Dir;
	}

	const float ClosestRadius = Capsule->GetScaledCapsuleRadius();
	FVector Start = GetActorLocation() + (Dir * ClosestRadius);

	const float HalfHeight = Capsule->GetScaledCapsuleHalfHeight();
	const float MaxStepHeight = MoveComp->MaxStepHeight;
	Start.Z = Start.Z - HalfHeight + MaxStepHeight + 0.01f;

	const float TraceLength = FMath::Max(MoveComp->Velocity.Size(), 100.f);

	const FVector End = Start + (Dir * TraceLength);

	FHitResult Hit;

	const bool bHit = World->LineTraceSingleByChannel(Hit, Start, End, TraceChannel);

	if (!bHit)
	{
		return ActionValue;
	}

	const float Ratio = FMath::Clamp(Hit.Distance / TraceLength, 0.f, 1.f);

	return (FMath::Abs(ActionValue * Ratio) > 0.1f) ? ActionValue * Ratio : 0.f;
}

float AEotRHumanCharacter::CalculateCameraInput(bool bIsYawAxis, float ActionValue) const
{
	float Delta;

	if (bIsYawAxis)
	{
		const float RootWorldYaw = GetMesh()->GetSocketRotation(TEXT("root")).Yaw;
		const float MeshRelativeYaw = GetMesh()->GetRelativeRotation().Yaw;
		const float CurrentYaw = RootWorldYaw - MeshRelativeYaw;

		const float TargetYaw = GetControlRotation().Yaw;

		Delta = UKismetMathLibrary::NormalizeAxis(TargetYaw) - UKismetMathLibrary::NormalizeAxis(CurrentYaw);
	}
	else
	{
		Delta = GetControlRotation().Pitch;
	}

	const float DeltaNorm = UKismetMathLibrary::NormalizeAxis(Delta);

	const float Alpha = FMath::Clamp(FMath::Abs(DeltaNorm) / 90, 0.f, 1.f);

	const float Scaled = (1.f - Alpha) * ActionValue;

	return (ActionValue * DeltaNorm * (bIsYawAxis ? -1.f : 1.f) > 0.0) ? ActionValue : Scaled;
}