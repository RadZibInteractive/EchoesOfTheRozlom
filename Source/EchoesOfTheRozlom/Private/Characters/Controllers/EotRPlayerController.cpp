#include "Characters/Controllers/EotRPlayerController.h"
#include "FrameworkBase/EotRLocalPlayerSubsystem.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "Components/EotRTraversalComponent.h"
#include "FrameworkBase/EotRGameplayTags.h"

void AEotRPlayerController::InitInputSystem()
{
	Super::InitInputSystem();

	const ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (!LocalPlayer)
	{
		return;
	}

	const UEotRLocalPlayerSubsystem* LocalPlayerSubsystem = LocalPlayer->GetSubsystem<UEotRLocalPlayerSubsystem>();
	if (!LocalPlayerSubsystem)
	{
		return;
	}

	const UInputMappingContext* InputMappingContext = LocalPlayerSubsystem->GetInputMappingContext();
	if (!InputMappingContext)
	{
		return;
	}

	if (UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
	{
		EnhancedInputSubsystem->AddMappingContext(InputMappingContext, 0);
	}
}

void AEotRPlayerController::UpdateRotation(float DeltaTime)
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)
	{
		Super::UpdateRotation(DeltaTime);
		return;
	}

	if (IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(ControlledPawn))
	{
		UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent();
		if (ASC && ASC->HasMatchingGameplayTag(EotRGameplayTags::State_Movement_Traversing))
		{
			const FRotator CurrentControlRot = GetControlRotation();
			const FVector TargetControlVec = ControlledPawn->GetActorForwardVector();
			
			FRotator TargetControlRot = TargetControlVec.Rotation();

			// TODO: Check posiibilities of ActorForwardVector != FrontLedgeNormal

		    /*
			if (UEotRTraversalComponent* TraversalComp = ControlledPawn->FindComponentByClass<UEotRTraversalComponent>())
			{
				FVector TargetNormal = TraversalComp->CachedResult.FrontLedgeNormal;
				if (TargetNormal.Normalize())
				{
					const float AdditiveAngle = FVector::DotProduct(-TargetNormal, TargetControlVec);
					const float AdditiveRad = FMath::Acos(AdditiveAngle);
					const float AdditiveDeg = FMath::RadiansToDegrees(AdditiveRad);
					TargetControlRot.Pitch += AdditiveDeg;
				}
			}
		    */

			const FRotator NewControlRot = FMath::RInterpTo(CurrentControlRot, TargetControlRot, DeltaTime, 3.0f);

			SetControlRotation(NewControlRot);
		}
	}

	Super::UpdateRotation(DeltaTime);
}