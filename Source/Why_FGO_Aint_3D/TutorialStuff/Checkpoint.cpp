// Fill out your copyright notice in the Description page of Project Settings.


#include "Checkpoint.h"
#include "Components/BoxComponent.h"
#include "GameFramework/GameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Why_FGO_Aint_3D/DefaultStageGamemode.h"
#include "Why_FGO_Aint_3D/Public/TutorialStuff/Tutorial_PlayerInterface.h"

// Sets default values
ACheckpoint::ACheckpoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxCollider = CreateDefaultSubobject<UBoxComponent>("Collider");
	RootComponent = BoxCollider; 
	SetRootComponent(BoxCollider);

	
}

// Called when the game starts or when spawned
void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();

	ADefaultStageGamemode* GM = Cast<ADefaultStageGamemode>(UGameplayStatics::GetGameMode(this));
	if (GM)
	{
		GM->RegisterCheckpoint(this);
	}
}

void ACheckpoint::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (BoxCollider)
	{
		BoxCollider->OnComponentBeginOverlap.RemoveDynamic(this, &ACheckpoint::CheckpointOverlapped);
		BoxCollider->OnComponentBeginOverlap.AddDynamic(this, &ACheckpoint::CheckpointOverlapped);
		BoxCollider->OnComponentEndOverlap.RemoveDynamic(this, &ACheckpoint::CheckpointOverlappedEnd);
		BoxCollider->OnComponentEndOverlap.AddDynamic(this, &ACheckpoint::CheckpointOverlappedEnd);
	}
}

// Called every frame
void ACheckpoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACheckpoint::DoInteract_Implementation()
{
	ITutorial_InteractionMessages::DoInteract_Implementation();
	UE_LOG(LogTemp, Log, TEXT("I've been interacted"));
	if (OnEndGameDelegate.IsBound())
	{
		OnEndGameDelegate.Broadcast(TEXT("EndGame"));
	}
}

void ACheckpoint::CheckpointOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp,Warning, TEXT("Overlapped"));

	if (UKismetSystemLibrary::DoesImplementInterface(OtherActor, UTutorial_PlayerInterface::StaticClass()))
	{
		UE_LOG(LogTemp,Warning, TEXT("Overlap actor has interface"));
		ITutorial_PlayerInterface::Execute_SetActorOverlap(OtherActor, this);
	}
}

void ACheckpoint::CheckpointOverlappedEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp,Warning, TEXT("Overlapped Ended"));

	if (UKismetSystemLibrary::DoesImplementInterface(OtherActor, UTutorial_PlayerInterface::StaticClass()))
	{
		UE_LOG(LogTemp,Warning, TEXT("Overlap actor has interface"));
		ITutorial_PlayerInterface::Execute_SetActorOverlap(OtherActor, nullptr);
	}
}

