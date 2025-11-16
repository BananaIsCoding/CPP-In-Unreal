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
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "Why_FGO_Aint_3D/MyGameStuff/DefaultPlayerBattleModeCpp.h"
#include "Why_FGO_Aint_3D/MyGameStuff/DefaultStageGamemodeV2.h"
#include "Why_FGO_Aint_3D/MyGameStuff/StandardFgoDamageType.h"


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
	HealthComponent->OnDamageDone.AddUniqueDynamic(this, &ABaseBattleEnemyCpp::UpdateHealthUI);
	HealthComponent->OnDeath.AddUniqueDynamic(this, &ABaseBattleEnemyCpp::OnEnemyDeath);

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
	AttackHitBox->SetupAttachment(RootComponent);
	AttackHitBox->SetRelativeLocation(FVector(90.0f, 0.0f, 0.0f));
	AttackHitBox->SetRelativeScale3D(FVector(1.75f, 1.75f, 2.0f ));
	AttackHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttackHitBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ABaseBattleEnemyCpp::OnAttackHitBoxOverlapBegin);

	UCapsuleComponent* BaseRootPart = Cast<UCapsuleComponent>(GetRootComponent());
	BaseRootPart->SetCollisionProfileName("DefaultEnemyBodyCollision");
}

void ABaseBattleEnemyCpp::SelectRandomTarget()
{
	int RandonPlayerIndex = FMath::RandRange(0, GameMode->GetPlayerCharacterCount() - 1);
	Target = &GameMode->GetPlayerCharacter(RandonPlayerIndex);
}

void ABaseBattleEnemyCpp::SelectRandomCardType()
{
	// + 1 because 0 is none
	HealthComponent->ChosenCard = static_cast<ECardType>(rand() % 3 + 1);
}

void ABaseBattleEnemyCpp::UpdateHealthUI()
{
	if (CurrentState == CardTurnMode)
	{
		UpdateCardTurnHpBar();
		SelectRandomCardType();
		UpdateCardDisplayer(HealthComponent->ChosenCard);
	}
	else
	{
		UpdateBattleTurnHpBar();
		if (!IsHitStun)
		{
			IsHitStun = true;
			RotatePoint->StopRotate();
			FTimerHandle AHandleToUse;
			WorldTimerManager->SetTimer(AHandleToUse, this, &ABaseBattleEnemyCpp::EndHitStun, 1.0f, false);
		}
		if (!IsBattleHpBarInList)
		{
			IsBattleHpBarInList = true;
			AddToBattleHpList();
			FTimerHandle TimerHandle;
			WorldTimerManager->SetTimer(TimerHandle, this, &ABaseBattleEnemyCpp::HideBattleHpBarFromList, 4.0f, false);
		}
	}
}

void ABaseBattleEnemyCpp::HideBattleHpBarFromList()
{
	BattleModeHPBar->RemoveFromParent();
	IsBattleHpBarInList = false;
}

void ABaseBattleEnemyCpp::EndHitStun()
{
	IsHitStun = false;
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

void ABaseBattleEnemyCpp::OnAttackHitBoxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<ADefaultPlayerBattleModeCpp>(OtherActor) == Target)
	{

		/*
		UStandardFgoDamageType FgoDamageType;
		FgoDamageType.ClassOfAttacker = EnemyInfo.Class;
		UGameplayStatics::ApplyDamage(Target, EnemyInfo.Damage, GetController(), this, FgoDamageType.StaticClass());
		*/

		/*
		FDamageEvent MyCustomDamageEvent;
		MyCustomDamageEvent.DamageTypeClass = GameMode->StandardDamageType;
		Cast<UStandardFgoDamageType>(MyCustomDamageEvent.DamageTypeClass)->Init(EnemyInfo.Class);
		Target->TakeDamage(EnemyInfo.Damage, MyCustomDamageEvent, GetController(), this);
		*/
		Target->HealthComponent->TakeDamage(EnemyInfo.Damage, EnemyInfo.Class);
	}
}

void ABaseBattleEnemyCpp::BeginPlay()
{
	Super::BeginPlay();
	
	GameMode = Cast<ADefaultStageGamemodeV2>( GetWorld()->GetAuthGameMode());
	WorldTimerManager = &GetWorld()->GetTimerManager();
	FTransform Transform = FTransform( FRotator::ZeroRotator, FVector(0, 0, -100), FVector::OneVector	);
	FActorSpawnParameters SpawnParams;
	RotatePoint = Cast<AActorRotator> (GetWorld()->SpawnActor(ActorRotatorClass, &Transform, SpawnParams));
	EnemyAIController = GetController<AAIController>();

	if (GameMode)
	{
		GameMode->OnCardPrepTurn.AddDynamic(this, &ABaseBattleEnemyCpp::PrepForCardTurn);
		GameMode->ShowEnemyTarget.AddDynamic(this, &ABaseBattleEnemyCpp::ChangeToAttackMode);
	}
	BattleModeHPBar = CreateWidget(GetWorld()->GetGameInstance(), Wb_BattleTurnEnemyHpBar);
	CardDisplayer = CardInfoSceneWidget->GetWidget();
	SetReferenceAndCastUiInBP();
}

void ABaseBattleEnemyCpp::StartBehaviourLoop()
{
	WorldTimerManager->SetTimer(BehaviorLoopTimerHandler, this, &ABaseBattleEnemyCpp::BehaviourLoop, 0.2, true);
	CardInfoSceneWidget->SetVisibility(false);

	WorldTimerManager->SetTimer(LookTimerHandler, this, &ABaseBattleEnemyCpp::LookAtPlayer, 0.01, true);
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
				EnemyAIController->MoveToActor(Target, 250.0f);;
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

	FVector DirectionVector = UKismetMathLibrary::GetDirectionUnitVector(Target->GetActorLocation(), GetActorLocation());

	BeforeAttackPos = (DirectionVector * 295.0f) + Target->GetActorLocation();

	Cast<UCapsuleComponent>(GetRootComponent())->SetCollisionProfileName("IgnoreEnemy");

	EnemyAIController->MoveToActor(Target, 100.0f);
	EnemyAIController->ReceiveMoveCompleted.AddDynamic(this, &ABaseBattleEnemyCpp::CheckOnAttackPos);
}

void ABaseBattleEnemyCpp::CheckOnAttackPos(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	EnemyAIController->ReceiveMoveCompleted.RemoveDynamic(this, &ABaseBattleEnemyCpp::CheckOnAttackPos);
	Cast<UCapsuleComponent>(GetRootComponent())->SetCollisionProfileName("DefaultEnemyBodyCollision");
	if (TargetIsNotNpc)
	{
		AttackHitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		FTimerHandle TimerHandle;
		WorldTimerManager->SetTimer(TimerHandle, this, &ABaseBattleEnemyCpp::EndAttack, 0.1f, false);
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
			HealthComponent->TakeDamage(Target->PlayerInfo.Damage, Target->PlayerInfo.Class);
		}
		else
		{
			Target->HealthComponent->TakeDamage(EnemyInfo.Damage, EnemyInfo.Class);
		}

		EndAttack();
	}
}

void ABaseBattleEnemyCpp::OnEnemyDeath()
{
	if (IsBattleHpBarInList)
		BattleModeHPBar->RemoveFromParent();
	GameMode->OnCardPrepTurn.RemoveDynamic(this, &ABaseBattleEnemyCpp::PrepForCardTurn);
	GameMode->ShowEnemyTarget.RemoveDynamic(this, &ABaseBattleEnemyCpp::ChangeToAttackMode);
	GameMode->OnEnemyDefeat(this);
}

void ABaseBattleEnemyCpp::PrepForCardTurn()
{
	CurrentState = CardTurnMode;
	WorldTimerManager->ClearTimer(BehaviorLoopTimerHandler);
	WorldTimerManager->ClearTimer(LookTimerHandler);
	RotatePoint->StopRotate();
	DetachFromActor(
		FDetachmentTransformRules(
		EDetachmentRule::KeepWorld,
		EDetachmentRule::KeepRelative,
		EDetachmentRule::KeepRelative,
		true
		)
	);
	// stop it from going somewhere else
	GetMovementComponent()->StopMovementImmediately();
	SelectRandomTarget();
	SelectRandomCardType();
	SetActorHiddenInGame(true);
	EnableEnemyCardDisplayer(true, Target->PlayerInfo.Name, HealthComponent->ChosenCard);
	CardInfoSceneWidget->SetVisibility(true);
}

void ABaseBattleEnemyCpp::DealCardDamage(float DamagePercentage)
{
	Target->HealthComponent->TakeDamage(EnemyInfo.Damage * (DamagePercentage/100.0f), EnemyInfo.Class, HealthComponent->ChosenCard);
}

void ABaseBattleEnemyCpp::ChangeToAttackMode()
{
	SelectRandomTarget();
	SelectRandomCardType();
	EnableEnemyCardDisplayer(false, Target->PlayerInfo.Name, HealthComponent->ChosenCard);
}

void ABaseBattleEnemyCpp::EndCardAttack(float AttackAnimDuration, bool DoYouNeedMeToDoDmg)
{
	SetActorHiddenInGame(true);
	if (DoYouNeedMeToDoDmg)
	{
		DealCardDamage(100);
	}
	FTimerHandle Handle;
	WorldTimerManager->SetTimer(Handle, GameMode, &ADefaultStageGamemodeV2::EnemyCardAttackCompleteStuff, AttackAnimDuration, false );
}

void ABaseBattleEnemyCpp::SetEnemyStats(FInfoStruct Info)
{
	EnemyInfo = Info;
	FString Nametag = EnemyInfo.Name.ToString();
	switch (EnemyInfo.Class)
	{
		case EFgoClassType::Saber:
			Nametag += "(Saber)";
		break;
		case EFgoClassType::Lancer:
			Nametag += "(Lancer)";
		break;
		case EFgoClassType::Archer:
			Nametag += "(Archer)";
		break;
		case EFgoClassType::Rider:
			Nametag += "(Rider)";
		break;
		case EFgoClassType::Caster:
			Nametag += "(Caster)";
		break;
		case EFgoClassType::Assassin:
			Nametag += "(Assassin)";
		break;
		case EFgoClassType::Berserker:
			Nametag += "(Berserker)";
		break;
		default:
			
		break;
	}
	
	TextComponent->SetText(FText::FromString(Nametag));
	HealthComponent->MaxHealth = EnemyInfo.Health;
	HealthComponent->CurrentHealth = EnemyInfo.Health;
	
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
			EnemyAIController->MoveToLocation(BeforeAttackPos, 25.0f);
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
	return IsPlayerInRange && !IsHitStun;
}

void ABaseBattleEnemyCpp::DoCardAttack()
{
	switch (HealthComponent->ChosenCard)
	{
		case ECardType::Quick:
			QuickAttack();
		break;
		case ECardType::Art:
			ArtAttack();
		break;
		case ECardType::Buster:
			BusterAttack();
		break;
		default:
		break;
	}
}

