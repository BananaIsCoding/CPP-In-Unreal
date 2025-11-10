// Fill out your copyright notice in the Description page of Project Settings.


#include "MyEnemyAiController.h"

#include "BaseEnemyCpp.h"


// Sets default values
AMyEnemyAiController::AMyEnemyAiController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AMyEnemyAiController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);
	ABaseEnemyCpp* Enemy = Cast<ABaseEnemyCpp>(GetPawn());
	Enemy->OnPathFollowFinished();
}

