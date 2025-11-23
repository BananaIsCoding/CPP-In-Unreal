// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemyCpp.h"
#include "Why_FGO_Aint_3D/MyGameStuff/DefaultStageGamemodeV2.h"
#include "MyEnemyAiController.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Why_FGO_Aint_3D/DefaultCharacter.h"
//#include "Perception/PawnSensingComponent.h"


// Sets default values
ABaseEnemyCpp::ABaseEnemyCpp()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//AIController = CreateDefaultSubobject<AAIController>(TEXT("AIController"));
	GetMesh()->SetCollisionProfileName("IgnoreEnemy");
	GetCapsuleComponent()->SetCollisionProfileName("IgnoreEnemy");

	AttackHitBox = CreateDefaultSubobject<UBoxComponent>("HitBox");
	AttackHitBox->SetupAttachment(RootComponent);
	AttackHitBox->SetCollisionProfileName("OverlapAll");
	AttackHitBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ABaseEnemyCpp::OnHitBoxHit);

	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	
}

// Called when the game starts or when spawned
void ABaseEnemyCpp::BeginPlay()
{
	Super::BeginPlay();

	if (EnemyInfo.Class == EFgoClassType::None)
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
	// + 1 because 0 is none
	EnemyInfo.Class = static_cast<EFgoClassType>(rand() % 7 + 1);
}

void ABaseEnemyCpp::PathFollowState()
{
	if (!IsPlayerDetected)
	{
		AActor* point = PathPointArray[CurrentPathPoint];
		GetController<AMyEnemyAiController>()->MoveToActor(point,5.0f,false);
		//AIController->MoveToActor(point,5.0f,false);
		//GetController<AAIController>()->ReceiveMoveCompleted.AddDynamic(this, &ABaseEnemyCpp::OnPathFollowFinished);
	}
}

void ABaseEnemyCpp::OnPathFollowFinished()
{
	if (!IsPlayerDetected)
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
}

void ABaseEnemyCpp::ChasePlayerState()
{
	GetController<AAIController>()->MoveToActor(GetWorld()->GetFirstPlayerController()->GetCharacter(),5.0f,false);
}

void ABaseEnemyCpp::OnSeePlayer()
{
	IsPlayerDetected = true;
	GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
	GetWorld()->GetTimerManager().SetTimer(LookTimerHandler, this, &ABaseEnemyCpp::ChasePlayerState, 0.2f, true);
}

void ABaseEnemyCpp::OnHitBoxHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ( Cast<ADefaultCharacter>( OtherActor))
	{
		DefaultStageGamemode->BattleSetUp(GetTransform());
	}
}



