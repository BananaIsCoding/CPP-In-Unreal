// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleZoneCpp.h"
#include "EnemyStuff/BaseBattleEnemyCpp.h"
#include "EnemyStuff/BaseEnemyCpp.h"
#include "GameFramework/GameModeBase.h"
#include "DefaultStageGamemodeV2.generated.h"

struct FCardPoolItem;
class ADefaultPlayerBattleModeCpp;
class AIntroCameraCpp;
class UUserWidget;
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

	TArray<ABaseEnemyCpp*> EnemiesToAdd;

	TArray<ABaseBattleEnemyCpp*> EnemiesArray;

	//TArray<TSubclassOf<ADefaultPlayerBattleModeCpp>*> PartyArray;
	TArray<ADefaultPlayerBattleModeCpp*> PartyArray;

	ADefaultPlayerBattleModeCpp* CurrentPossessedChar;

	TArray<FCardPoolItem> CardPool;

	float PosOffset;

	ABattleZoneCpp* BattleZoneBp;

	void CalcOffset(int ArrayLen);

	FTransform GetSpawnPosition(FRotator Rotation, float XSpawnOffset, int Index);
	
	void SetUpIntroCamera(FRotator Rotation, float XSpawnOffset, int Index);
	
	virtual void BeginPlay() override;

	void IntroEnemy();

	void IntroPlayer();
	
public:
	void BattleSetUp(FTransform BattleStartPos);
	void PreBattleIntro();
	void AddEnemyToBattle(ABaseEnemyCpp* EnemyToAdd);

	

	UPROPERTY(EditDefaultsOnly, Category = "Required Objects")
	TSubclassOf<ABattleZoneCpp> BattleZoneClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Required Objects")
	TSubclassOf<AIntroCameraCpp> IntroCamClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Stuff For Designers")
	int SpawnSpacing = 200;
	
	UPROPERTY(EditAnywhere, Category = "Stuff For Designers")
	float IntroCutsceneMultiplier = 2.0f;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> Wb_PartyMenu;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	class UUserWidget* PartyMenu;
};
