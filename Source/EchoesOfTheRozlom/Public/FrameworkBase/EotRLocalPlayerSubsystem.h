// © 2026 RadZib. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "EotRLocalPlayerSubsystem.generated.h"

class UEotRInputTagConfig;
class UInputMappingContext;

UCLASS()
class ECHOESOFTHEROZLOM_API UEotRLocalPlayerSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	// ULocalPlayerSubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// Input
	const UInputMappingContext* GetInputMappingContext() const { return InputMappingContext; }
	const UEotRInputTagConfig* GetInputTagConfig() const { return InputTagConfig; }

private:
	// Input
	UPROPERTY(Transient)
	TObjectPtr<UInputMappingContext> InputMappingContext = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UEotRInputTagConfig> InputTagConfig = nullptr;
};