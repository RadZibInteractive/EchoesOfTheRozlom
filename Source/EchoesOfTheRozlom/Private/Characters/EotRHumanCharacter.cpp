#include "Characters/EotRHumanCharacter.h"

#include "FrameworkBase/EotRLocalPlayerSubsystem.h"
#include "Data/DataAssets/EotRInputTagConfig.h"

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


void AEotRHumanCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	const UEotRHumanDataAsset* HumanData = Cast<UEotRHumanDataAsset>(CharacterData);

	if (!HumanData)
	{
		return;
	}

	FirstPersonCameraSocket = HumanData->FirstPersonCameraSocketName;
	WeaponSocket = HumanData->WeaponSocketName;

	if (HumanData->UpperMeshAsset)
	{
		UpperMesh->SetSkeletalMesh(HumanData->UpperMeshAsset);
	}

	if (HumanData->UpperAnimInstanceClass)
	{
		UpperMesh->SetAnimInstanceClass(HumanData->UpperAnimInstanceClass);
	}

	if (HumanData->HeadMeshAsset)
	{
		HeadMesh->SetSkeletalMesh(HumanData->HeadMeshAsset);
	}

	for (int32 i = 0; i < HumanData->PartMeshesAssets.Num(); ++i)
	{
		const FName PartName = *FString::Printf(TEXT("PartMesh_%d"), i);
		USkeletalMeshComponent* NewComp =
			NewObject<USkeletalMeshComponent>(this, USkeletalMeshComponent::StaticClass(), PartName);

		NewComp->RegisterComponent();
		NewComp->AttachToComponent(UpperMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		NewComp->SetSkeletalMesh(HumanData->PartMeshesAssets[i]);
		NewComp->SetLeaderPoseComponent(UpperMesh);
		NewComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		PartMeshes.Add(NewComp);
	}

	FirstPersonCamera->AttachToComponent(
		UpperMesh,
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		FirstPersonCameraSocket
	);
	FirstPersonCamera->SetRelativeRotation(FRotator(0.f, 90.f, -90.f));
}

void AEotRHumanCharacter::SetAbilityAnimTarget(bool bUseAlternative)
{
	if (FGameplayAbilityActorInfo* ActorInfo = AbilitySystemComponent->AbilityActorInfo.Get())
	{
		if (bUseAlternative)
		{
			ActorInfo->SkeletalMeshComponent = UpperMesh;
			ActorInfo->AffectedAnimInstanceTag = FName(TEXT("Upper"));
		}
		else
		{
			ActorInfo->SkeletalMeshComponent = GetMesh();
			ActorInfo->AffectedAnimInstanceTag = NAME_None;
		}
	}
}

// Sets default values
AEotRHumanCharacter::AEotRHumanCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Setup
	USkeletalMeshComponent* LowerMesh = GetMesh();

	UpperMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("UpperMesh"));
	UpperMesh->SetupAttachment(LowerMesh);
	UpperMesh->AddTickPrerequisiteComponent(LowerMesh);
	LowerMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LowerMesh->SetVisibility(false);
	LowerMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;

	HeadMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HeadMesh"));
	HeadMesh->SetupAttachment(UpperMesh);
	HeadMesh->SetLeaderPoseComponent(UpperMesh);
	HeadMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HeadMesh->SetOwnerNoSee(true);
	HeadMesh->bCastHiddenShadow = true;

	FirstPersonCamera = CreateDefaultSubobject<UEotRCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(UpperMesh);
	FirstPersonCamera->bUsePawnControlRotation = true;
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

	const float CapsuleRadius = Capsule->GetScaledCapsuleRadius();
	FVector Start = GetActorLocation() + (Dir * CapsuleRadius);

	const float HalfHeight = Capsule->GetScaledCapsuleHalfHeight();
	const float MaxStepHeight = MoveComp->MaxStepHeight;
	Start.Z = Start.Z - HalfHeight + MaxStepHeight + 0.01f;

	const float TraceLength = FMath::Max(MoveComp->Velocity.Size(), 100.f);

	const FVector End = Start + (Dir * TraceLength);

	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(CalculateMoveInputTrace), false);
	Params.AddIgnoredActor(this);

	const bool bHit = World->LineTraceSingleByChannel(Hit, Start, End, TraceChannel, Params);

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