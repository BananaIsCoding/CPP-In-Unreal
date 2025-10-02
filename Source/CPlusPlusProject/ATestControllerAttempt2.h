// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CPlusPlusProjectPlayerController.h"
#include "ATestControllerAttempt2.generated.h"

/**
 * 
 */
UCLASS()
class CPLUSPLUSPROJECT_API AATestControllerAttempt2 : public ACPlusPlusProjectPlayerController
{
	GENERATED_BODY()
public:
	AATestControllerAttempt2();
protected:
	virtual void BeginPlay() override;
};
