// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultStageGamemodeV2.h"

ADefaultStageGamemodeV2::ADefaultStageGamemodeV2()
{
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

FTransform ADefaultStageGamemodeV2::GetSpawnPosition(FRotator Rotation, float XSpawnOffset, int Index)
{
	FVector BattleZoneLocation = BattleZoneBp->GetActorLocation();
	FVector NewLocation = FVector(BattleZoneLocation.X + XSpawnOffset,(SpawnSpacing * Index) + XSpawnOffset + BattleZoneLocation.Y, BattleZoneLocation.Z);

	return FTransform(Rotation.Quaternion(), NewLocation, FVector::One());
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

void ADefaultStageGamemodeV2::BattleSetUp(FTransform BattleStartPos)
{
	EngagePos = BattleStartPos;
	FActorSpawnParameters SpawnParameters;
	BattleZoneBp = Cast<ABattleZoneCpp>(GetWorld()->SpawnActor(BattleZoneClass,&EngagePos, SpawnParameters));
	int Index = 0;
	for (ABaseEnemyCpp* Enemy : EnemiesToAdd)
	{
		FTransform EnemySpawnTransform = GetSpawnPosition(FRotator(0, 180, 0),500.0f, Index);
		ABaseBattleEnemyCpp* NewBattleEnemy = Cast<ABaseBattleEnemyCpp>(GetWorld()->
			SpawnActor(
				Enemy->BattleEnemyClass,
				&EnemySpawnTransform,
				SpawnParameters
			)
		);
		
		//ABaseBattleEnemyCpp* NewBattleEnemy = GetWorld()->SpawnActor<ABaseBattleEnemyCpp>();
		//NewBattleEnemy->SetActorTransform(FTransform::Identity, false);
		//NewBattleEnemy->SetActorTransform(GetSpawnPosition(FRotator(0, 0, 180),500.0f, Index));
		EnemiesArray.Add(NewBattleEnemy);
		Index += 1;
	}
}

void ADefaultStageGamemodeV2::PreBattleIntro()
{
	
}
