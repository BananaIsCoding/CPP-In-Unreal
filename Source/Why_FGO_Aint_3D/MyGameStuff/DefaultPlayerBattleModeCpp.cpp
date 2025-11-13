// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultPlayerBattleModeCpp.h"
#include "DefaultStageGamemodeV2.h"
#include "AI/NavigationSystemBase.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextRenderComponent.h"


// Sets default values
ADefaultPlayerBattleModeCpp::ADefaultPlayerBattleModeCpp()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TextComponent = CreateDefaultSubobject<UTextRenderComponent>("NameTag");
	TextComponent->SetupAttachment(RootComponent);
	TextComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	TextComponent->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;
	
}

void ADefaultPlayerBattleModeCpp::BeginPlay()
{
	Super::BeginPlay();

	TextComponent->SetText(FText::FromName(PlayerInfo.Name));

	DefaultStageGamemode = Cast<ADefaultStageGamemodeV2>(GetWorld()->GetAuthGameMode());
	
	HealthComponent->MaxHealth = PlayerInfo.Health;
	HealthComponent->CurrentHealth = PlayerInfo.Health;

	PartyMenuHPBar = CreateWidget(GetWorld()->GetGameInstance(), Wb_PartyMenuHpItem);

	SetUpPartyMenuHPBar();
}

void ADefaultPlayerBattleModeCpp::AddEnemyToManager(ABaseBattleEnemyCpp* Enemy)
{
	EnemyArray.Add(Enemy);
}

void ADefaultPlayerBattleModeCpp::StartEnemyManager(bool IsControlledByPlayer)
{
	ControlledByPlayer = IsControlledByPlayer;
	// Update Healthbar
	CurrentEnemyIndex = 0;

	GetWorld()->GetTimerManager().SetTimer(ManagerTimerHandle, this, &ADefaultPlayerBattleModeCpp::MakeEnemyAttack, 2.5f, false);
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
			}
			CurrentEnemyIndex++;
		}
		else
		{
			EnemyArray.RemoveAt(CurrentEnemyIndex);
		}

		GetWorld()->GetTimerManager().SetTimer(WaitTimerHandle, this, &ADefaultPlayerBattleModeCpp::MakeEnemyAttack, 0.1f, false);
		
	}
}

void ADefaultPlayerBattleModeCpp::StopEnemyManager()
{
	ManagerTimerHandle.Invalidate();
	WaitTimerHandle.Invalidate();
	EnemyArray.Empty();
}

