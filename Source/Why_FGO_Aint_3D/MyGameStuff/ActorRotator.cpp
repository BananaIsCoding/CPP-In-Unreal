// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorRotator.h"

#include "EnemyStuff/BaseBattleEnemyCpp.h"
#include "GameFramework/RotatingMovementComponent.h"


// Sets default values
AActorRotator::AActorRotator()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = SceneComponent;

	RotatingMovementComponent = CreateDefaultSubobject<URotatingMovementComponent>("RotatingMovement");
	
}

void AActorRotator::StartRotate()
{
	float RotateSpeed = FMath::RandRange(15.0f, 30.0f);
	RotateSpeed *= 1 - (2 * FMath::RandRange(0.0f, 1.0f));

	RotatingMovementComponent->RotationRate.Yaw = RotateSpeed;

	GetWorld()->GetTimerManager().SetTimer(RotateTimer, this, &AActorRotator::StopRotate, FMath::RandRange(5.0f, 15.0f), false);
}

void AActorRotator::StopRotate()
{
	RotateTimer.Invalidate();
	RotatingMovementComponent->RotationRate = FRotator(0.0f, 0.0f, 0.0f);

	TArray<AActor*> ChildrenArray;
	this->GetAttachedActors(ChildrenArray, true, false);
	for (AActor* Child : ChildrenArray)
	{
		if (ABaseBattleEnemyCpp* Enemy = Cast<ABaseBattleEnemyCpp>(Child); Enemy != nullptr)
		{
			Enemy->StrafeEnded();
		}
	}
}

void AActorRotator::ChangeDirection()
{
	RotatingMovementComponent->RotationRate.Yaw *= -1.0f;
}

