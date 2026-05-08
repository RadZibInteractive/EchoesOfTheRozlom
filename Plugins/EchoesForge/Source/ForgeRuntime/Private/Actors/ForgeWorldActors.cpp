// © 2026 RadZib. All rights reserved.

#include "Actors/ForgeWorldActors.h"
#include "Components/SceneComponent.h"

AForgeNPCActor::AForgeNPCActor()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

AForgeAnomalyActor::AForgeAnomalyActor()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

AForgeEnemyActor::AForgeEnemyActor()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

AForgePropActor::AForgePropActor()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}
