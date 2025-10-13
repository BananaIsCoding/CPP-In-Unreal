// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TutorialStuff/Tutorial_InteractionMessages.h"
#include "Checkpoint.generated.h"

class UBoxComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEndGameSigniture, FString, EndMessage);

UCLASS()
class WHY_FGO_AINT_3D_API ACheckpoint : public AActor, public  ITutorial_InteractionMessages
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACheckpoint();

	UPROPERTY(EditAnywhere)
	UBoxComponent* BoxCollider;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual void DoInteract_Implementation() override;

	UPROPERTY(BlueprintAssignable)
	FEndGameSigniture OnEndGameDelegate;

private:
	
	UFUNCTION()
	void CheckpointOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void CheckpointOverlappedEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
