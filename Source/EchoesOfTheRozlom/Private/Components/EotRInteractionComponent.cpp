// © 2026 RadZib. All rights reserved.

#include "Components/EotRInteractionComponent.h"
#include "Characters/EotRHumanCharacter.h"
#include "Components/EotRCameraComponent.h"
#include "Engine/World.h"
#include "Interfaces/EotRInteractableInterface.h"

UEotRInteractionComponent::UEotRInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UEotRInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	PerformInteractionTrace();
}

void UEotRInteractionComponent::PerformInteractionTrace(ECollisionChannel TraceChannel)
{
	AEotRHumanCharacter* EotRCharacter = Cast<AEotRHumanCharacter>(GetOwner());
	if (!EotRCharacter) return;

	const UCameraComponent* CameraComp = EotRCharacter->GetFirstPersonCamera();
	if (!CameraComp) return;

	const UWorld* World = GetWorld();
	if (!World) return;

	FVector Start = CameraComp->GetComponentLocation();
	FVector CameraDirection = CameraComp->GetForwardVector();
	FVector End = Start + CameraDirection * 500.f;

	FHitResult Hit;
	const FCollisionShape Sphere = FCollisionShape::MakeSphere(25.f);

	const bool bHit = World->SweepSingleByChannel(Hit, Start, End, FQuat::Identity, TraceChannel, Sphere);

	AActor* NewFocusedActor = nullptr;

	if (bHit)
	{
		AActor* HitActor = Hit.GetActor();

		if (HitActor && HitActor->Implements<UEotRInteractableInterface>())
		{
			NewFocusedActor = HitActor;
		}
	}

	if (NewFocusedActor != FocusedActor)
	{
		if (IsValid(FocusedActor) && FocusedActor->Implements<UEotRInteractableInterface>())
		{
			IEotRInteractableInterface::Execute_OnFocusLost(FocusedActor);
		}

		FocusedActor = NewFocusedActor;

		if (IsValid(FocusedActor) && FocusedActor->Implements<UEotRInteractableInterface>())
		{
			IEotRInteractableInterface::Execute_OnFocusGained(FocusedActor);
		}
	}
}

void UEotRInteractionComponent::TryInteract()
{
	AEotRHumanCharacter* EotRCharacter = Cast<AEotRHumanCharacter>(GetOwner());
	if (!EotRCharacter) return;

	if (IsValid(FocusedActor) && FocusedActor->Implements<UEotRInteractableInterface>())
	{
		IEotRInteractableInterface::Execute_Interact(FocusedActor, EotRCharacter);
	}
}