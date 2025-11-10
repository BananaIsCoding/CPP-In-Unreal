// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleZoneCpp.h"
#include "EnemyStuff/BaseEnemyCpp.h"
#include "GameFramework/GameModeBase.h"
#include "DefaultStageGamemodeV2.generated.h"

/**
 * 
 */
UCLASS()
class WHY_FGO_AINT_3D_API ADefaultStageGamemodeV2 : public AGameModeBase
{
	GENERATED_BODY()
	ADefaultStageGamemodeV2();

protected:
	FTransform EngagePos;


	void IntroEnemy();

	void CalcOffset(int ArrayLen);
	
	TArray<ABaseEnemyCpp*> EnemiesToAdd;

	float PosOffset;


public:
	void BattleSetUp(FTransform BattleStartPos);
	void PreBattleIntro();
	void AddEnemyToBattle(ABaseEnemyCpp* EnemyToAdd);

	UPROPERTY(EditDefaultsOnly, Category = "Stuff For Designers")
	TSubclassOf<ABattleZoneCpp> BattleZoneClass;

	UPROPERTY(EditDefaultsOnly, Category = "Stuff For Designers")
	int SpawnSpacing = 200;
	
	
};
