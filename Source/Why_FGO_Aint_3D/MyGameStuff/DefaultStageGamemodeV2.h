// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

public:
	void BattleSetUp(FTransform BattleStartPos);
	void PreBattleIntro();
	
	
};
