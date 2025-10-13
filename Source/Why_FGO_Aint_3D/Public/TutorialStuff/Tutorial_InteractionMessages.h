// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Tutorial_InteractionMessages.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UTutorial_InteractionMessages : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class WHY_FGO_AINT_3D_API ITutorial_InteractionMessages
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void DoInteract();
};
