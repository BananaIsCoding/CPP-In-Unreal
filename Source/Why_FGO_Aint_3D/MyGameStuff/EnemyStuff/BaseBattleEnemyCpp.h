// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Why_FGO_Aint_3D/MyGameStuff/Structs/InfoStruct.h"
#include "BaseBattleEnemyCpp.generated.h"

class ADefaultPlayerBattleModeCpp;
class UHealthComponent;
class UTextRenderComponent;
class UUserWidget;

UCLASS()
class WHY_FGO_AINT_3D_API ABaseBattleEnemyCpp : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseBattleEnemyCpp();
	

protected:
	
	enum EEnemyState
	{
		None,
		MoveToPlayer,
		Strafing,
		Attacking,
		CardTurnMode
	};
	
	UPROPERTY(EditAnywhere)
	class UTextRenderComponent* TextComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UHealthComponent> HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI")
	class UUserWidget* BattleModeHPBar;

	ADefaultPlayerBattleModeCpp* Target;

	EEnemyState CurrentState = None;

	void StartBehaviourLoop();
	
public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stuff For Designers")
	FInfoStruct EnemyInfo;
	
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> Wb_BattleTurnEnemyHpBar;

	void SetEnemyStats(FInfoStruct Info);

	void ActivateEnemy(ADefaultPlayerBattleModeCpp* TheTarget);
	
	UFUNCTION(BlueprintImplementableEvent)
	void SetUpBattleTurnHPBar();

	
};
