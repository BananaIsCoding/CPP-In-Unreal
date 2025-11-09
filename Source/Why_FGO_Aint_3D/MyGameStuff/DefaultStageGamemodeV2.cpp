// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultStageGamemodeV2.h"

ADefaultStageGamemodeV2::ADefaultStageGamemodeV2()
{
}

void ADefaultStageGamemodeV2::BattleSetUp(FTransform BattleStartPos)
{
	EngagePos = BattleStartPos;
	//GetWorld()->SpawnActor
}
