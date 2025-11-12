// Fill out your copyright notice in the Description page of Project Settings.


#include "IntroCameraCpp.h"

#include "Camera/CameraComponent.h"

// Sets default values
AIntroCameraCpp::AIntroCameraCpp()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = SceneComponent;
	
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

void AIntroCameraCpp::StartMovingCamera()
{
	GetWorld()->GetTimerManager().SetTimer(MoveCameraTimerHandler, this, &AIntroCameraCpp::MoveCamera, 0.01f, true);
}

void AIntroCameraCpp::MoveCamera()
{
	//UE_LOG(LogTemp, Log, TEXT("Hello"));
	
	if (Camera->GetRelativeLocation() != EndPoint->GetRelativeLocation())
	{
		Camera->SetRelativeLocation(
			FMath::VInterpTo(
				Camera->GetRelativeLocation(),
				EndPoint->GetRelativeLocation(),
				0.01f,
				0.3
			)
		);
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(MoveCameraTimerHandler);
	}
}

void AIntroCameraCpp::StartTheCutscene(float TravelDistance)
{
	FVector NewPoint = EndPoint->GetRelativeLocation();
	NewPoint.Y = TravelDistance / 2;
	EndPoint->SetRelativeLocation(NewPoint);

	NewPoint = StartPoint->GetRelativeLocation();
	NewPoint.Y = (TravelDistance / 2) * -1;
	StartPoint->SetRelativeLocation(NewPoint);

	Camera->SetRelativeLocation(StartPoint->GetRelativeLocation());
	GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(this);
	StartMovingCamera();
}



