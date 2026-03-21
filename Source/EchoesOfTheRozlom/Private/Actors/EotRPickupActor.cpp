// © 2026 RadZib. All rights reserved.

#include "Actors/EotRPickupActor.h"
#include "Data/DataAssets/EotRItemDataAsset.h"
#include "Characters/EotRHumanCharacter.h"

AEotRPickupActor::AEotRPickupActor()
{
	PrimaryActorTick.bCanEverTick = false;

	PickupMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	PickupMeshComponent->SetupAttachment(RootComponent);
	PickupMeshComponent->SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
	PickupMeshComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Block);
	PickupMeshComponent->SetSimulatePhysics(true);
}

void AEotRPickupActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (!ItemData)
	{
		return;
	}

	ItemInstance.ItemData = ItemData;

	if (ItemData->PickupMesh)
	{
		PickupMeshComponent->SetStaticMesh(ItemData->PickupMesh);
	}

	if (ItemData->MaxStackCount)
	{
		ItemInstance.StackCount = ItemData->MaxStackCount;
	}
}

void AEotRPickupActor::OnFocusGained_Implementation()
{
	if (GEngine)
	{
		FString Msg = FString::Printf(TEXT("Focus gained: %s"), *this->GetName());

		GEngine->AddOnScreenDebugMessage(
			-1,
			2.f,
			FColor::Green,
			Msg
		);
	}
}

void AEotRPickupActor::OnFocusLost_Implementation()
{
	if (GEngine)
	{
		FString Msg = FString::Printf(TEXT("Focus lost: %s"), *this->GetName());

		GEngine->AddOnScreenDebugMessage(
			-1,
			2.f,
			FColor::Red,
			Msg
		);
	}
}

void AEotRPickupActor::Interact_Implementation(AActor* Interactor)
{
	AEotRHumanCharacter* EotRCharacter = Cast<AEotRHumanCharacter>(Interactor);
	if (!EotRCharacter) return;

	// Just instant destroy for now
	Destroy();
}