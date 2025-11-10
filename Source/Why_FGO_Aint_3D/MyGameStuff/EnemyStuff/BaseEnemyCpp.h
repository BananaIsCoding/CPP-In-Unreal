// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Why_FGO_Aint_3D/MyGameStuff/Structs/InfoStruct.h"
#include "EnemyWalkPointCpp.h"
#include "Why_FGO_Aint_3D/MyGameStuff/DefaultStageGamemodeV2.h"
#include "AIController.h"
#include "BaseEnemyCpp.generated.h"

//class UPawnSensingComponent;
class UBoxComponent;
UCLASS()
class WHY_FGO_AINT_3D_API ABaseEnemyCpp : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseEnemyCpp();

protected:

	//class UPawnSensingComponent* PawnSensingComponent;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess="true"))
	class UBoxComponent* AttackHitBox;

	UPROPERTY(EditAnywhere, Category = "Adjustable Variables")
	float ChaseSpeed = 450.0f;
	
	AAIController* AIController;
	
	FInfoStruct EnemyInfo;

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

	void OnPathFollowFinished(FAIRequestID RequestID, EPathFollowingResult::Type Result);

	void ChasePlayerState();

	void OnSeePlayer();

public:
	
};
