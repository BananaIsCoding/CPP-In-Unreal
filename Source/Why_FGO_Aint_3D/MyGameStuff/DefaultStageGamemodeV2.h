// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleZoneCpp.h"
#include "EnemyStuff/BaseBattleEnemyCpp.h"
#include "EnemyStuff/BaseEnemyCpp.h"
#include "Enums/FgoCardTypeEnum.h"
#include "GameFramework/GameModeBase.h"
#include "DefaultStageGamemodeV2.generated.h"

class UStandardFgoDamageType;
struct FCardPoolItem;
class ADefaultPlayerBattleModeCpp;
class AIntroCameraCpp;
class UTestHpWidget;
class UUserWidget;
class ADefaultCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBattleModeEnded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPrepForCardTurn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEnemyCardAttackTurn);

UCLASS()
class WHY_FGO_AINT_3D_API ADefaultStageGamemodeV2 : public AGameModeBase
{
	GENERATED_BODY()
	ADefaultStageGamemodeV2();

protected:
	enum EBattleState
	{
		UnEngaged,
		SettingUp,
		InBattleTurn,
		InCardTurn
	};

	EBattleState CurrentBattleState = EBattleState::UnEngaged;
	
	// to stop code searching for key things every time it needs it
	FTimerManager* WorldTimerManager;
	APlayerController* PlayerController;
	ADefaultCharacter* FreeRoamCharacter;
	UWorld* TheWorld;
	
	FTransform EngagePos;
	ABattleZoneCpp* BattleZoneBp;
	
	float PosOffset;

	// enemies and players
	TArray<ABaseEnemyCpp*> EnemiesToAdd;
	TArray<ABaseBattleEnemyCpp*> EnemiesArray;
	TArray<ADefaultPlayerBattleModeCpp*> PartyArray;
	ADefaultPlayerBattleModeCpp* CurrentPossessedChar;
	
	int ViewingEnemyIndex = 0;
	AIntroCameraCpp* CardTurnCamera;

	TArray<FCardPoolItem> CardPool;
	TArray<int, TFixedAllocator<5>> CardPoolInUse;
	int PlayerAttackCount;

	void CalcOffset(int ArrayLen);
	FTransform GetSpawnPosition(FRotator Rotation, float XSpawnOffset, int Index);
	void SetUpIntroCamera(FRotator Rotation, float XSpawnOffset, int Index);
	void EnemyTargetPicker();
	void ViewEnemy();
	//void ViewSelectedEnemy(int Index);

	virtual void BeginPlay() override;

	void IntroEnemy();
	void IntroPlayer();
	void IntroCutsceneWaiter(int Num);

	void BattleTurn();
	void CardTurnSetUp();
public:

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnBattleModeEnded OnBattleEnded;
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FPrepForCardTurn OnCardPrepTurn;
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FEnemyCardAttackTurn ShowEnemyTarget;
	
	UPROPERTY(EditDefaultsOnly, Category = "Required Objects")
	TSubclassOf<ABattleZoneCpp> BattleZoneClass;
	UPROPERTY(EditDefaultsOnly, Category = "Required Objects")
	TSubclassOf<AIntroCameraCpp> IntroCamClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Stuff For Designers")
	int SpawnSpacing = 200;
	UPROPERTY(EditAnywhere, Category = "Stuff For Designers")
	float IntroCutsceneMultiplier = 2.0f;
	UPROPERTY(EditAnywhere, Category = "Stuff For Designers")
	float BattleTurnDuration = 45.0f;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> Wb_PartyMenu;
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	class UUserWidget* PartyMenu;
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> Wb_MainHpBarUI;
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	class UUserWidget* MainHpBar;
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> Wb_EnemyBattleHpListUI;
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	class UUserWidget* EnemyBattleHpList;
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> Wb_CardSelectionUI;
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	class UUserWidget* CardSelectionList;
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> Wb_EnemySelectionMenuUI;
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	class UUserWidget* EnemySelectionMenu;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UTestHpWidget> Wb_MainHpBarUiCpp;
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	UTestHpWidget* CppMainHpBar;

	UFUNCTION(BlueprintImplementableEvent)
	void SetReferenceAndCastUiInBP();
	
	ADefaultPlayerBattleModeCpp& GetPlayerCharacter(int Index) { return *PartyArray[Index]; };
	int GetPlayerCharacterCount() { return PartyArray.Num(); };
	
	void BattleSetUp(FTransform BattleStartPos);
	void PreBattleIntro();
	void AddEnemyToBattle(ABaseEnemyCpp* EnemyToAdd);
	
	UFUNCTION(BlueprintCallable)
	void ChangeCharPossess(ADefaultPlayerBattleModeCpp* PlayerCharacter);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateEnemyViewUI(UPARAM() float CurrentHp , UPARAM(ref) FInfoStruct& EnemyInfo);
	UFUNCTION(BlueprintCallable)
	void ChangeEnemyView(UPARAM() bool GoBackward);
	UFUNCTION(BlueprintImplementableEvent)
	void CLearCardSelection();
	UFUNCTION(BlueprintImplementableEvent)
	void ChangeToDefenceMode();
	UFUNCTION(BlueprintImplementableEvent)
	void PassCardInfo(UPARAM() FName CharName, UPARAM() ECardType AttackType, UPARAM() int IndexInPoolArray, UPARAM() int CardNum);
	UFUNCTION(BlueprintCallable)
	void ChosenCard(UPARAM() int IndexInPoolArray);
	UFUNCTION()
	void PlayerCardAttackCompleteStuff();
	UFUNCTION()
	void EnemyCardAttackCompleteStuff();
	void DelayEnemyAttackAnim();
	void DealDamageToCurrentViewingEnemy(float Damage, EFgoClassType ClassType, ECardType CardType);

	void OnEnemyDefeat(ABaseBattleEnemyCpp* DefeatedEnemy);
	void OnCharacterDefeat(ADefaultPlayerBattleModeCpp* PlayerChar);
	void EndBattleMode();
};
