// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MyEnemyAiController.generated.h"

UCLASS()
class WHY_FGO_AINT_3D_API AMyEnemyAiController : public AAIController
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMyEnemyAiController();

protected:
	// Called when the game starts or when spawned
	void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;


};
