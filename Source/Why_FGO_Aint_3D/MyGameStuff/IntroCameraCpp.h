// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IntroCameraCpp.generated.h"

class UCameraComponent;
UCLASS()
class WHY_FGO_AINT_3D_API AIntroCameraCpp : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AIntroCameraCpp();
	
protected:

	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> StartPoint;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> EndPoint;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UCameraComponent> Camera;

public:
	void StartTheCutscene(float travelDistance);
	
};
