// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseBossEnemy.h"

#include "Blueprint/UserWidget.h"
#include "Why_FGO_Aint_3D/MyGameStuff/DefaultStageGamemodeV2.h"


// Sets default values
ABaseBossEnemy::ABaseBossEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ABaseBossEnemy::OnEnemyDeath()
{
	if (IsBattleHpBarInList)
		BattleModeHPBar->RemoveFromParent();
	GameMode->OnCardPrepTurn.RemoveDynamic(this, &ABaseBossEnemy::PrepForCardTurn);
	GameMode->ShowEnemyTarget.RemoveDynamic(this, &ABaseBossEnemy::ChangeToAttackMode);
	GameMode->OnBossDefeat();
}

void ABaseBossEnemy::PrepForCardTurn()
{
	Super::PrepForCardTurn();
}

void ABaseBossEnemy::ChangeToAttackMode()
{
	Super::ChangeToAttackMode();
}

void ABaseBossEnemy::SetUpGameModeDelegateLink()
{
	GameMode->OnCardPrepTurn.AddDynamic(this, &ABaseBossEnemy::PrepForCardTurn);
	GameMode->ShowEnemyTarget.AddDynamic(this, &ABaseBossEnemy::ChangeToAttackMode);
}



