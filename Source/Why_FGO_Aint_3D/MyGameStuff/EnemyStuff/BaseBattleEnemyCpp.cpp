// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseBattleEnemyCpp.h"

#include "AIController.h"
#include "Blueprint/UserWidget.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "TutorialStuff/HealthComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Navigation/PathFollowingComponent.h"
#include "Why_FGO_Aint_3D/MyGameStuff/ActorRotator.h"
#include "AITypes.h"
#include "Why_FGO_Aint_3D/MyGameStuff/DefaultPlayerBattleModeCpp.h"


// Sets default values
ABaseBattleEnemyCpp::ABaseBattleEnemyCpp()
{
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	
	/*SceneComponent = CreateDefaultSubobject<USceneComponent>("SceneComponent");
	RootComponent = SceneComponent;*/
	TextComponent = CreateDefaultSubobject<UTextRenderComponent>("NameTag");
	TextComponent->SetupAttachment(RootComponent);
	TextComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	TextComponent->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

	AttackRangeHitBox = CreateDefaultSubobject<USphereComponent>("AttackRange");
	AttackRangeHitBox->SetupAttachment(RootComponent);
	AttackRangeHitBox->SetSphereRadius(300.0);
	AttackRangeHitBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ABaseBattleEnemyCpp::OnAttackRangeOverlapBegin);
	AttackRangeHitBox->OnComponentEndOverlap.AddUniqueDynamic(this, &ABaseBattleEnemyCpp::OnAttackingOverlapEnd);

	CardInfoSceneWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("CardInfoWidget"));
	CardInfoSceneWidget->SetupAttachment(RootComponent);
	CardInfoSceneWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 145.0f));
	CardInfoSceneWidget->SetRelativeScale3D(FVector(1.0f, .5f, .5f));

	AttackHitBox = CreateDefaultSubobject<UBoxComponent>("Attack HitBox");
	AttackHitBox->SetRelativeLocation(FVector(90.0f, 0.0f, 0.0f));
	AttackHitBox->SetRelativeScale3D(FVector(1.75f, 1.75f, 2.0f ));
	AttackHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

void ABaseBattleEnemyCpp::OnAttackRangeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (CurrentState != CardTurnMode)
	{
		if (Cast<ADefaultPlayerBattleModeCpp>(OtherActor) == Target)
		{
			IsPlayerInRange = true;

			RotatePoint->SetActorLocation(Target->GetActorLocation());
			FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules(
				EAttachmentRule::KeepWorld,
				EAttachmentRule::KeepRelative,
				EAttachmentRule::KeepRelative,
				true
			);
		
			AttachToActor(RotatePoint, AttachmentRules);
			
			CurrentState = Strafing;
			
		}
	}
}

void ABaseBattleEnemyCpp::OnAttackingOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (CurrentState != CardTurnMode)
	{
		if (Cast<ADefaultPlayerBattleModeCpp>(OtherActor) == Target)
		{
			IsPlayerInRange = false;
		
			DetachFromActor(
				FDetachmentTransformRules(
				EDetachmentRule::KeepWorld,
				EDetachmentRule::KeepRelative,
				EDetachmentRule::KeepRelative,
				true
				)
			);
		
			CurrentState = MoveToPlayer;
			StrafeEnded();
		}
	}
}

void ABaseBattleEnemyCpp::BeginPlay()
{
	Super::BeginPlay();

	FTransform Transform = FTransform( FRotator::ZeroRotator, FVector(0, 0, -100), FVector::OneVector	);
	FActorSpawnParameters SpawnParams;
	RotatePoint = Cast<AActorRotator> (GetWorld()->SpawnActor(ActorRotatorClass, &Transform, SpawnParams));
}

void ABaseBattleEnemyCpp::StartBehaviourLoop()
{
	GetWorld()->GetTimerManager().SetTimer(BehaviorLoopTimerHandler, this, &ABaseBattleEnemyCpp::BehaviourLoop, 0.2, true);
	CardInfoSceneWidget->SetVisibility(false);

	GetWorld()->GetTimerManager().SetTimer(LookTimerHandler, this, &ABaseBattleEnemyCpp::LookAtPlayer, 0.01, true);
}

void ABaseBattleEnemyCpp::BehaviourLoop()
{
	if (!IsHitStun)
	{
		switch (CurrentState)
		{
			case Strafing:
				if (IsStrafeEnded)
				{
					IsStrafeEnded = false;
					RotatePoint->StartRotate();
				}
			break;
			case MoveToPlayer:
				RotatePoint->StopRotate();
				GetController<AAIController>()->MoveToActor(Target, 250.0f);;
			break;
			default:
			break;
		}
	}

	
}

void ABaseBattleEnemyCpp::LookAtPlayer()
{
	SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target->GetActorLocation()));
}

void ABaseBattleEnemyCpp::MoveToAttackPos()
{
	CurrentState = Attacking;
	RotatePoint->StopRotate();
	// might not need to call strafe ended and stop rotates calls it
	StrafeEnded();

	BeforeAttackPos = GetActorLocation();

	Cast<UCapsuleComponent>(GetRootComponent())->SetCollisionProfileName("IgnoreEnemy");

	AAIController* ControllerRef = GetController<AAIController>();
	ControllerRef->MoveToActor(Target, 100.0f);
	ControllerRef->ReceiveMoveCompleted.AddUniqueDynamic(this, &ABaseBattleEnemyCpp::CheckOnAttackPos);
	
}

void ABaseBattleEnemyCpp::CheckOnAttackPos(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	GetController<AAIController>()->ReceiveMoveCompleted.RemoveDynamic(this, &ABaseBattleEnemyCpp::CheckOnAttackPos);
	Cast<UCapsuleComponent>(GetRootComponent())->SetCollisionProfileName("DefaultEnemyBodyCollision");
	if (Result == EPathFollowingResult::Success)
	{
		if (TargetIsNotNpc)
		{
			AttackHitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ABaseBattleEnemyCpp::EndAttack, 0.1f, false);
		}
		else
		{
			int RandNum = FMath::RandRange(0,99);
			if (RandNum < 50)
			{
				UE_LOG(LogTemp, Log, TEXT("Block"));
			}
			else if (RandNum < 70)
			{
				//HealthComponent->CurrentHealth -= EnemyInfo.Damage;
				UE_LOG(LogTemp, Log, TEXT("Enemy hit themselves"));
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("Hit Player"));
			}

			EndAttack();
		}
	}
}

void ABaseBattleEnemyCpp::SetEnemyStats(FInfoStruct Info)
{
	EnemyInfo = Info;
	FString Nametag = EnemyInfo.Name.ToString() + "(" + EnemyInfo.Class.ToString() + ")";
	
	TextComponent->SetText(FText::FromString(Nametag));
	HealthComponent->MaxHealth = EnemyInfo.Health;
	HealthComponent->CurrentHealth = EnemyInfo.Health;

	BattleModeHPBar = CreateWidget(GetWorld()->GetGameInstance(), Wb_BattleTurnEnemyHpBar);
	
	SetUpBattleTurnHPBar();
}

void ABaseBattleEnemyCpp::ActivateEnemy(ADefaultPlayerBattleModeCpp* TheTarget)
{
	Target = TheTarget;
	CurrentState = MoveToPlayer;

	SetActorHiddenInGame(false);

	StartBehaviourLoop();

	// reset the detection (mainly to detect if target is alr in range)
	AttackRangeHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttackRangeHitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	
}

void ABaseBattleEnemyCpp::StrafeEnded()
{
	IsStrafeEnded = true;
}

void ABaseBattleEnemyCpp::AttackPlayer()
{
	TargetIsNotNpc = true;
	MoveToAttackPos();
}

void ABaseBattleEnemyCpp::AttackChar()
{
	TargetIsNotNpc = false;
	MoveToAttackPos();
}

void ABaseBattleEnemyCpp::EndAttack()
{
	AttackHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (CurrentState != CardTurnMode)
	{
		if (IsPlayerInRange)
		{
			GetController<AAIController>()->MoveToLocation(BeforeAttackPos, 25.0f);
			CurrentState = Strafing;
		}
		else
		{
			CurrentState = MoveToPlayer;
		}
	}
}

bool ABaseBattleEnemyCpp::CanEnemyAttack()
{
	if (IsPlayerInRange)
	{
	}
	return IsPlayerInRange;
}

