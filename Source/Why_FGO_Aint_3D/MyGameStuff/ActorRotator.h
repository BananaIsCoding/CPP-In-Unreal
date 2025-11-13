// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActorRotator.generated.h"

class URotatingMovementComponent;

UCLASS()
class WHY_FGO_AINT_3D_API AActorRotator : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AActorRotator();

protected:

	UPROPERTY(EditAnywhere)
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(EditAnywhere)
	TObjectPtr<URotatingMovementComponent> RotatingMovementComponent;

	FTimerHandle RotateTimer;
	
public:

	void StartRotate();
	void StopRotate();
	void ChangeDirection();
};
