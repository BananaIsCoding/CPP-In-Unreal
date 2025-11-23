// Fill out your copyright notice in the Description page of Project Settings.


#include "MyEnemyAiController.h"

#include "BaseEnemyCpp.h"


// Sets default values
AMyEnemyAiController::AMyEnemyAiController()
{
	PrimaryActorTick.bCanEverTick = true;
}
// at first my delegate did not work, so I made my own controller to override this function.
// (I later got the delegate to work for a different thing but didn't have time to change it so this is not use)
void AMyEnemyAiController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);
	ABaseEnemyCpp* Enemy = Cast<ABaseEnemyCpp>(GetPawn());
	Enemy->OnPathFollowFinished();
}

