// Fill out your copyright notice in the Description page of Project Settings.


#include "IntroCameraCpp.h"

#include "Camera/CameraComponent.h"

// Sets default values
AIntroCameraCpp::AIntroCameraCpp()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(RootComponent);
	Camera->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));

	StartPoint = CreateDefaultSubobject<UStaticMeshComponent>("StartPoint");
	StartPoint->SetupAttachment(RootComponent);
	StartPoint->SetRelativeLocation(FVector(0.0f, -180.0f, 50.0f));

	EndPoint = CreateDefaultSubobject<UStaticMeshComponent>("EndPoint");
	EndPoint->SetupAttachment(RootComponent);
	EndPoint->SetRelativeLocation(FVector(0.0f, 180.0f, 50.0f));
	
}



