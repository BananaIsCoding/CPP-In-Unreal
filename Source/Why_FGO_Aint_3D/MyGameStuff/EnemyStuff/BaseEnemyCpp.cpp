// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemyCpp.h"

#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"
//#include ""


// Sets default values
ABaseEnemyCpp::ABaseEnemyCpp()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//AIController = CreateDefaultSubobject<AAIController>(TEXT("AIController"));
	AIController = GetController<AAIController>();

	AttackHitBox = CreateDefaultSubobject<UBoxComponent>("HitBox");
}

// Called when the game starts or when spawned
void ABaseEnemyCpp::BeginPlay()
{
	Super::BeginPlay();

	if (EnemyInfo.Class == "None")
	{
		SetRandomClass();
	}
	DefaultStageGamemode = (ADefaultStageGamemodeV2*) GetWorld()->GetAuthGameMode();

	if (!PathPointArray.IsEmpty())
	{
		PathFollowState();
	}
}

void ABaseEnemyCpp::SetRandomClass()
{
	switch (rand() % 7)
	{
	case 0:
		EnemyInfo.Class = "Saber";
		break;
	case 1:
		EnemyInfo.Class = "Lancer";
		break;
	case 2:
		EnemyInfo.Class = "Archer";
		break;
	case 3:
		EnemyInfo.Class = "Caster";
		break;
	case 4:
		EnemyInfo.Class = "Assassin";
		break;
	case 5:
		EnemyInfo.Class = "Rider";
		break;
	case 6:
		EnemyInfo.Class = "Berserker";
		break;
	default:
		
		break;
	}
}

void ABaseEnemyCpp::PathFollowState()
{
	if (!IsPlayerDetected)
	{
		AActor* point = PathPointArray[CurrentPathPoint];
		AIController->MoveToActor(point,5.0f,false);
		AIController->ReceiveMoveCompleted.AddDynamic(this, &ABaseEnemyCpp::OnPathFollowFinished);
	}
}

void ABaseEnemyCpp::OnPathFollowFinished(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	if (CurrentPathPoint < PathPointArray.Num() - 1)
	{
		CurrentPathPoint++;
	}
	else
	{
		CurrentPathPoint = 0;
	}
	PathFollowState();
}

void ABaseEnemyCpp::ChasePlayerState()
{
	
	
}

void ABaseEnemyCpp::OnSeePlayer()
{
	IsPlayerDetected = true;
	GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
	GetWorld()->GetTimerManager().SetTimer(LookTimerHandler, this, &ABaseEnemyCpp::ChasePlayerState, 0.2f, true);
}




