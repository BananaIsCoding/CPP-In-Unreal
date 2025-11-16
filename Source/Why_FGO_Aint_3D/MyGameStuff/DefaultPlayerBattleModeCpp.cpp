// Fill out your copyright notice in the Description page of Project Settings.
#include "DefaultPlayerBattleModeCpp.h"
#include "DefaultStageGamemodeV2.h"
#include "AI/NavigationSystemBase.h"
#include "Blueprint/UserWidget.h"
#include "Components/BoxComponent.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/TestHpWidget.h"

// Sets default values
ADefaultPlayerBattleModeCpp::ADefaultPlayerBattleModeCpp()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TextComponent = CreateDefaultSubobject<UTextRenderComponent>("NameTag");
	TextComponent->SetupAttachment(RootComponent);
	TextComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	TextComponent->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);

	HealthComponent->OnDamageDone.AddUniqueDynamic(this, &ADefaultPlayerBattleModeCpp::UpdateHealthUI);
	HealthComponent->OnDeath.AddUniqueDynamic(this, &ADefaultPlayerBattleModeCpp::OnCharDeath);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	AttackHitBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ADefaultPlayerBattleModeCpp::OnAttackHitBoxOverlay);
}

void ADefaultPlayerBattleModeCpp::BeginPlay()
{
	Super::BeginPlay();

	DefaultStageGamemode = Cast<ADefaultStageGamemodeV2>(GetWorld()->GetAuthGameMode());
	PlayerController = GetWorld()->GetFirstPlayerController();
	WorldTimerManager = &GetWorld()->GetTimerManager();

	TextComponent->SetText(FText::FromName(PlayerInfo.Name));

	HealthComponent->MaxHealth = PlayerInfo.Health;
	HealthComponent->CurrentHealth = PlayerInfo.Health;
	HealthComponent->ClassOfDefender = PlayerInfo.Class;

	if (DefaultStageGamemode)
	{
		DefaultStageGamemode->OnBattleEnded.AddDynamic(this, &ADefaultPlayerBattleModeCpp::DisableCharacter);
		DefaultStageGamemode->OnCardPrepTurn.AddDynamic(this, &ADefaultPlayerBattleModeCpp::PrepareForCardTurn);
	}
	
	PartyMenuHPBar = CreateWidget(GetWorld()->GetGameInstance(), Wb_PartyMenuHpItem);
	SetReferenceAndCastUiInBP();
}

void ADefaultPlayerBattleModeCpp::UpdateHealthUI()
{
	UpdatePartyMenuHpUI();
	if (ControlledByPlayer)
	{
		//UpdateMainHpUI();
		DefaultStageGamemode->CppMainHpBar->UpdateHealthBar(HealthComponent->CurrentHealth, HealthComponent->MaxHealth);
	}
}

void ADefaultPlayerBattleModeCpp::OpenPartyMenu()
{
	if (ControlledByPlayer)
	{
		if (MenuOpened)
		{
			UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1);
			PlayerController->SetShowMouseCursor(false);
			PlayerController->SetInputMode(FInputModeGameOnly());
			DefaultStageGamemode->PartyMenu->RemoveFromParent();
		}
		else
		{
			UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0);
			PlayerController->SetShowMouseCursor(true);
			PlayerController->SetInputMode(FInputModeGameAndUI());
			DefaultStageGamemode->PartyMenu->AddToViewport();
		}
		MenuOpened = !MenuOpened;
	}
}

void ADefaultPlayerBattleModeCpp::DoCardAttack(int CardNumber)
{
	ChosenCardIndex = CardNumber;
	switch (CardNumber)
	{
		case 0:
			CardAttack1Stuff();
		break;
		case 1:
			CardAttack2Stuff();
		break;
		case 2:
			CardAttack3Stuff();
		break;
		case 3:
			CardAttack4Stuff();
		break;
		case 4:
			CardAttack5Stuff();
		break;
		default:
		break;
	}
}

void ADefaultPlayerBattleModeCpp::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* Input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		Input->BindAction(TabPressedAction, ETriggerEvent::Triggered, this, &ADefaultPlayerBattleModeCpp::OpenPartyMenu);
	}
}

void ADefaultPlayerBattleModeCpp::AttackFunction_Implementation()
{
	if (!MenuOpened)
	{
		// my own attack thing (Comment out if using designer stuff)
		AttackHitBox->SetCollisionProfileName("PlayerHitBox");

		FTimerHandle Handle;
		WorldTimerManager->SetTimer(Handle, this, &ADefaultPlayerBattleModeCpp::EndAttack, 0.1f, false);

		// this will call designer's attack stuff
		AttackFunction();
	}
}

void ADefaultPlayerBattleModeCpp::EndAttack()
{
	AttackHitBox->SetCollisionProfileName("NoCollision");
}

void ADefaultPlayerBattleModeCpp::OnAttackHitBoxOverlay(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ABaseBattleEnemyCpp* Enemy = Cast<ABaseBattleEnemyCpp>(OtherActor))
	{
		Enemy->HealthComponent->TakeDamage(PlayerInfo.Damage, PlayerInfo.Class);
	}
}

void ADefaultPlayerBattleModeCpp::OnCharDeath()
{
	DefaultStageGamemode->OnBattleEnded.RemoveDynamic(this, &ADefaultPlayerBattleModeCpp::DisableCharacter);
	DefaultStageGamemode->OnCardPrepTurn.RemoveDynamic(this, &ADefaultPlayerBattleModeCpp::PrepareForCardTurn);
	DefaultStageGamemode->OnCharacterDefeat(this);
}

void ADefaultPlayerBattleModeCpp::PrepareForCardTurn()
{
	StopEnemyManager();
	SetActorHiddenInGame(true);
}

void ADefaultPlayerBattleModeCpp::EndCardAttack(float AttackAnimDuration, bool DoYouNeedMeToDoDmg)
{
	if (DoYouNeedMeToDoDmg)
	{
		DealCardDamage(100);
	}
	FTimerHandle Handle;
	WorldTimerManager->SetTimer(Handle, DefaultStageGamemode, &ADefaultStageGamemodeV2::PlayerCardAttackCompleteStuff, AttackAnimDuration, false );
}

void ADefaultPlayerBattleModeCpp::DealCardDamage(float DamagePercentage)
{
	float DamageToDeal = 0;
	switch (CardSkillList[ChosenCardIndex])
	{
	case ECardType::Quick:
		DamageToDeal = PlayerInfo.Damage * 0.8f;
		break;
	case ECardType::Art:
		DamageToDeal = PlayerInfo.Damage;
		break;
	case ECardType::Buster:
		DamageToDeal = PlayerInfo.Damage * 1.2f;
		break;
	default:
		break;
	}
	DefaultStageGamemode->DealDamageToCurrentViewingEnemy(DamageToDeal * (DamagePercentage/100.0f), PlayerInfo.Class, HealthComponent->ChosenCard);
}

void ADefaultPlayerBattleModeCpp::DisableCharacter()
{
	Super::DisableCharacter();
	PartyMenuHPBar->RemoveFromParent();
}

void ADefaultPlayerBattleModeCpp::EnableCharacter()
{
	Super::EnableCharacter();
	AddHpBarToList();
}

void ADefaultPlayerBattleModeCpp::AddEnemyToManager(ABaseBattleEnemyCpp* Enemy)
{
	EnemyArray.Add(Enemy);
}

void ADefaultPlayerBattleModeCpp::StartEnemyManager(bool IsControlledByPlayer)
{
	ControlledByPlayer = IsControlledByPlayer;
	UpdateHealthUI();
	CurrentEnemyIndex = 0;

	WorldTimerManager->SetTimer(ManagerTimerHandle, this, &ADefaultPlayerBattleModeCpp::MakeEnemyAttack, 5.0f, false);
}

void ADefaultPlayerBattleModeCpp::MakeEnemyAttack()
{
	if (EnemyArray.IsEmpty())
	{
		StopEnemyManager();
	}
	else
	{
		if (CurrentEnemyIndex == EnemyArray.Num())
		{
			CurrentEnemyIndex = 0;
		}

		if (EnemyArray[CurrentEnemyIndex] != nullptr)
		{
			if (ABaseBattleEnemyCpp* Enemy = EnemyArray[CurrentEnemyIndex]; Enemy->CanEnemyAttack())
			{
				if (ControlledByPlayer)
				{
					Enemy->AttackPlayer();
				}
				else
				{
					Enemy->AttackChar();
				}
				CurrentEnemyIndex++;
				WorldTimerManager->SetTimer(ManagerTimerHandle, this, &ADefaultPlayerBattleModeCpp::MakeEnemyAttack, 5.0f, false);
			}
		}
		else
		{
			EnemyArray.RemoveAt(CurrentEnemyIndex);
			WorldTimerManager->SetTimer(WaitTimerHandle, this, &ADefaultPlayerBattleModeCpp::MakeEnemyAttack, 0.1f, false);
		}
	}
}

void ADefaultPlayerBattleModeCpp::StopEnemyManager()
{
	WorldTimerManager->ClearTimer(ManagerTimerHandle);
	WorldTimerManager->ClearTimer(WaitTimerHandle);
	EnemyArray.Empty();
}