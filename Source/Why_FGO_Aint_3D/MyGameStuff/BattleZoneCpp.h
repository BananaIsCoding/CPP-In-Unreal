// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BattleZoneCpp.generated.h"

class USphereComponent;
class ADefaultStageGamemodeV2;

UCLASS()

class WHY_FGO_AINT_3D_API ABattleZoneCpp : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> SphereCollider;

	ADefaultStageGamemodeV2* DefaultStageGamemode;

public:
	// Sets default values for this actor's properties
	ABattleZoneCpp();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void StartIntro();
	
public:
	//UFUNCTION()
	//void OnComponentOverlay(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnHitBoxOverlay
	(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult &SweepResult
	);
};
