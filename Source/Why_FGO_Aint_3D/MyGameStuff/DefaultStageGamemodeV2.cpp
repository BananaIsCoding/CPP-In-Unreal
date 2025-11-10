// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultStageGamemodeV2.h"

ADefaultStageGamemodeV2::ADefaultStageGamemodeV2()
{
}

void ADefaultStageGamemodeV2::AddEnemyToBattle(ABaseEnemyCpp* EnemyToAdd)
{
	EnemiesToAdd.Add(EnemyToAdd);
	EnemyToAdd->Destroy();
}

void ADefaultStageGamemodeV2::IntroEnemy()
{
	TArray<ABaseEnemyCpp*> TempArray = EnemiesToAdd;
	EnemiesToAdd.Empty();
	CalcOffset(TempArray.Num());
}

void ADefaultStageGamemodeV2::CalcOffset(int ArrayLen)
{
	if (ArrayLen == 1)
	{
		PosOffset = 0;
	}
	else
	{
		PosOffset = (ArrayLen / 2) * (SpawnSpacing * -1);
		if (ArrayLen % 2 == 0)
		{
			PosOffset += SpawnSpacing / 2;
		}
	}
}

void ADefaultStageGamemodeV2::BattleSetUp(FTransform BattleStartPos)
{
	EngagePos = BattleStartPos;
	FActorSpawnParameters SpawnParameters;
	GetWorld()->SpawnActor(BattleZoneClass,&EngagePos, SpawnParameters);
}

void ADefaultStageGamemodeV2::PreBattleIntro()
{
	
}
