// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseBattleEnemyCpp.h"
#include "BaseBossEnemy.generated.h"

UCLASS()
class WHY_FGO_AINT_3D_API ABaseBossEnemy : public ABaseBattleEnemyCpp
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseBossEnemy();

protected:
	virtual void OnEnemyDeath() override;
	virtual void PrepForCardTurn() override;
	virtual void ChangeToAttackMode() override;
	virtual void SetUpGameModeDelegateLink() override;
};
