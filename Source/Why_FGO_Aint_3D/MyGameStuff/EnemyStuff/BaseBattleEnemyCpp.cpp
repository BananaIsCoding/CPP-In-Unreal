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
	CardInfoSceneWidget->SetVisibility(false);
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

// selects a random enemy in battle
void ABaseBattleEnemyCpp::SelectRandomTarget()
{
	int RandonPlayerIndex = FMath::RandRange(0, GameMode->GetPlayerCharacterCount() - 1);
	Target = &GameMode->GetPlayerCharacter(RandonPlayerIndex);
}
// Generate a random card for enemy to use
void ABaseBattleEnemyCpp::SelectRandomCardType()
{
	// + 1 because 0 is none
	HealthComponent->ChosenCard = static_cast<ECardType>(rand() % 3 + 1);
}
// Function to run enemy on damage code
void ABaseBattleEnemyCpp::UpdateHealthUI()
{
	if (CurrentState == CardTurnMode)
	{
		// update card turn UIs
		UpdateCardTurnHpBar();
		SelectRandomCardType();
		UpdateCardDisplayer(HealthComponent->ChosenCard);
	}
	else
	{
		UpdateBattleTurnHpBar();
		if (!IsHitStun)
		{
			// stuns the enemy for a brief moment and don't stack it
			IsHitStun = true;
			RotatePoint->StopRotate();
			FTimerHandle AHandleToUse;
			WorldTimerManager->SetTimer(AHandleToUse, this, &ABaseBattleEnemyCpp::EndHitStun, 1.0f, false);
		}
		if (!IsBattleHpBarInList)
		{
			// show the enemy health on the side of the screen for a brief moment
			// [NOTE] reset timer if hit again and info is still on screen 
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
// Set up rotation point to prepare to strafe around the player when they are in the attack range
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
// unbind the enemy from the rotator and change it's state make it move to the player
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
// deal damage to player if attack hitbox touches the player
void ABaseBattleEnemyCpp::OnAttackHitBoxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<ADefaultPlayerBattleModeCpp>(OtherActor) == Target)
	{
		Target->HealthComponent->TakeDamage(EnemyInfo.Damage, EnemyInfo.Class);
	}
}
// setup
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
		SetUpGameModeDelegateLink();
	}
	BattleModeHPBar = CreateWidget(GetWorld()->GetGameInstance(), Wb_BattleTurnEnemyHpBar);
	CardDisplayer = CardInfoSceneWidget->GetWidget();
	SetReferenceAndCastUiInBP();
}
void ABaseBattleEnemyCpp::SetUpGameModeDelegateLink()
{
	GameMode->OnCardPrepTurn.AddDynamic(this, &ABaseBattleEnemyCpp::PrepForCardTurn);
	GameMode->ShowEnemyTarget.AddDynamic(this, &ABaseBattleEnemyCpp::ChangeToAttackMode);
}

// behaviour loop stuff
void ABaseBattleEnemyCpp::StartBehaviourLoop()
{
	WorldTimerManager->SetTimer(BehaviorLoopTimerHandler, this, &ABaseBattleEnemyCpp::BehaviourLoop, 0.2, true);
	CardInfoSceneWidget->SetVisibility(false);
	// start function so that enemy always look at player
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
					GetCapsuleComponent()->SetCollisionProfileName("IgnoreEnemy");
					RotatePoint->StartRotate();
				}
			break;
			case MoveToPlayer:
				GetCapsuleComponent()->SetCollisionProfileName("DefaultEnemyBodyCollision");
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
	StrafeEnded();
	// calculate a reasonable distance for enemy to go back to after attacking the player
	BeforeAttackPos = GetActorLocation();
	FVector DirectionVector = UKismetMathLibrary::GetDirectionUnitVector(Target->GetActorLocation(), GetActorLocation());
	BeforeAttackPos = (DirectionVector * 295.0f) + Target->GetActorLocation();

	GetCapsuleComponent()->SetCollisionProfileName("IgnoreEnemy");

	EnemyAIController->MoveToActor(Target, 100.0f);
	EnemyAIController->ReceiveMoveCompleted.AddDynamic(this, &ABaseBattleEnemyCpp::CheckOnAttackPos);
}
// when enemy reaches the attack point
void ABaseBattleEnemyCpp::CheckOnAttackPos(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	EnemyAIController->ReceiveMoveCompleted.RemoveDynamic(this, &ABaseBattleEnemyCpp::CheckOnAttackPos);
	if (TargetIsNotNpc)
	{
		AttackHitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		FTimerHandle TimerHandle;
		WorldTimerManager->SetTimer(TimerHandle, this, &ABaseBattleEnemyCpp::EndAttack, 0.1f, false);
	}
	else
	{
		// randomise the outcome of the attack if enemy is attack player's ally
		int RandNum = FMath::RandRange(0,99);
		if (RandNum < 50)
		{
			//UE_LOG(LogTemp, Log, TEXT("Block"));
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
// when it is enemy's turn during card phrase
void ABaseBattleEnemyCpp::ChangeToAttackMode()
{
	SelectRandomTarget();
	SelectRandomCardType();
	EnableEnemyCardDisplayer(false, Target->PlayerInfo.Name, HealthComponent->ChosenCard);
}
// for designers to call to end the attack animations
// and pass through the damage percentage that was not done during the animation
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
// used to give enemy their data
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
// enable enemies to attack again during battle phrase
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
// both func below is used to determine if enemy is attack the player or their allies
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
// used by enemy manager to check if enemy can attack
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

