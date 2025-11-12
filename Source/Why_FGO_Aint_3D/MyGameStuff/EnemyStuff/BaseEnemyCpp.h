// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Why_FGO_Aint_3D/MyGameStuff/Structs/InfoStruct.h"
#include "EnemyWalkPointCpp.h"
#include "AIController.h"
#include "BaseEnemyCpp.generated.h"

//class UPawnSensingComponent;
class UBoxComponent;
class ADefaultStageGamemodeV2;
class ABaseBattleEnemyCpp;
UCLASS()
class WHY_FGO_AINT_3D_API ABaseEnemyCpp : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseEnemyCpp();

protected:

	//class UPawnSensingComponent* PawnSensingComponent;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> AttackHitBox;

	UPROPERTY(EditAnywhere, Category = "Adjustable Variables")
	float ChaseSpeed = 450.0f;
	
	//AAIController* AIController;

	bool IsPlayerDetected;

	int CurrentPathPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Walk Points")
 	TArray<AEnemyWalkPointCpp*> PathPointArray;

	ADefaultStageGamemodeV2* DefaultStageGamemode;

	FTimerHandle LookTimerHandler;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	void SetRandomClass();

	void PathFollowState();

	void ChasePlayerState();

	UFUNCTION(BlueprintCallable)
	void OnSeePlayer();

	UFUNCTION()
	void OnHitBoxHit
	(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult &SweepResult
	);
	
	UFUNCTION(BlueprintCallable)
	void OnHitBoxHitForBP(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                      int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	void OnPathFollowFinished();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stuff For Designers")
	FInfoStruct EnemyInfo;

	UPROPERTY(EditAnywhere, Category = "Stuff For Designers")
	TSubclassOf<ABaseBattleEnemyCpp> BattleEnemyClass;
};
