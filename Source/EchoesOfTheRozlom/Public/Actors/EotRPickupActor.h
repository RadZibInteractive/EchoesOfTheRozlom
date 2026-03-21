// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/EotRInteractableInterface.h"
#include "Data/Structs/EotRItemInstance.h"

class UEotRItemDataAsset;
class UEotRInteractableComponent;
class UStaticMeshComponent;

#include "EotRPickupActor.generated.h"

UCLASS()
class ECHOESOFTHEROZLOM_API AEotRPickupActor : public AActor, public IEotRInteractableInterface
{
	GENERATED_BODY()
	
public:	
	AEotRPickupActor();

	// EotRInteractableInterface
	virtual void OnFocusGained_Implementation() override;
	virtual void OnFocusLost_Implementation() override;
	virtual void Interact_Implementation(AActor* Interactor) override;

protected:
	// AActor
	virtual void PostInitializeComponents() override;

protected:
	// Data
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Default")
	TObjectPtr<UEotRItemDataAsset> ItemData = nullptr;

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default")
	TObjectPtr<UEotRInteractableComponent> InteractableComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default")
	TObjectPtr<UStaticMeshComponent> PickupMeshComponent = nullptr;

private:
	// Data
	UPROPERTY()
	FEotRItemInstance ItemInstance;
};
