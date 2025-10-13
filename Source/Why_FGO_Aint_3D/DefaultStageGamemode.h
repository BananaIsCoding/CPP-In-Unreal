// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Why_FGO_Aint_3D/TutorialStuff/Checkpoint.h"
#include "DefaultStageGamemode.generated.h"

/**
 * 
 */
UCLASS()
class WHY_FGO_AINT_3D_API ADefaultStageGamemode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ADefaultStageGamemode();

	void RegisterCheckpoint(ACheckpoint* CheckPointActor);

	UFUNCTION()
	void FinishGame(FString EndMessage);
};
