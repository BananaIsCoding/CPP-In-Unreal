// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Why_FGO_Aint_3D/MyGameStuff/Enums/FgoCardTypeEnum.h"
#include "Why_FGO_Aint_3D/MyGameStuff/Structs/InfoStruct.h"
#include "BaseBattleEnemyCpp.generated.h"

class AAIController;
class ADefaultStageGamemodeV2;
struct FAIRequestID;
namespace EPathFollowingResult
{
	enum Type : int;
}
class UBoxComponent;
class UWidgetComponent;
class AActorRotator;
class USphereComponent;
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
		MoveToPlayer,
		Strafing,
		Attacking,
		CardTurnMode
	};

	// to stop code searching for key things every time it needs it
	FTimerManager* WorldTimerManager;
	AActorRotator* RotatePoint;
	AAIController* EnemyAIController;
	UPROPERTY(BlueprintReadOnly)
	ADefaultStageGamemodeV2* GameMode;
	
	
	UPROPERTY(EditAnywhere)
	class UTextRenderComponent* TextComponent;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USphereComponent> AttackRangeHitBox;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UWidgetComponent> CardInfoSceneWidget;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> AttackHitBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	class UUserWidget* BattleModeHPBar;
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	class UUserWidget* CardDisplayer;

	EEnemyState CurrentState = CardTurnMode;

	bool IsPlayerInRange = false;

	FTimerHandle BehaviorLoopTimerHandler;
	FTimerHandle LookTimerHandler;

	FVector BeforeAttackPos;
	bool IsStrafeEnded = true;
	bool IsHitStun = false;
	bool TargetIsNotNpc;

	bool IsBattleHpBarInList = false;

	void SelectRandomTarget();
	void SelectRandomCardType();

	UFUNCTION()
	void UpdateHealthUI();
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateBattleTurnHpBar();
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateCardTurnHpBar();
	UFUNCTION(BlueprintImplementableEvent)
	void AddToBattleHpList();
	void HideBattleHpBarFromList();
	void EndHitStun();
	
	UFUNCTION()
	void OnAttackRangeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnAttackingOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnAttackHitBoxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	

	virtual void BeginPlay() override;
	virtual void SetUpGameModeDelegateLink();
	
	void StartBehaviourLoop();
	void BehaviourLoop();

	void LookAtPlayer();
	void MoveToAttackPos();
	UFUNCTION()
	void CheckOnAttackPos(FAIRequestID RequestID, EPathFollowingResult::Type Result);

	UFUNCTION()
	virtual void OnEnemyDeath();
	UFUNCTION()
	virtual void PrepForCardTurn();
	UFUNCTION(BlueprintImplementableEvent)
	void EnableEnemyCardDisplayer(UPARAM() bool HideTarget, UPARAM() FName TargetName, UPARAM() ECardType CardType);
	UFUNCTION(BlueprintImplementableEvent)
	void QuickAttack();
	UFUNCTION(BlueprintImplementableEvent)
	void ArtAttack();
	UFUNCTION(BlueprintImplementableEvent)
	void BusterAttack();
	UFUNCTION(BlueprintCallable)
	void EndCardAttack(UPARAM() float AttackAnimDuration, UPARAM() bool DoYouNeedMeToDoDmg);
	UFUNCTION(BlueprintCallable)
	void DealCardDamage(UPARAM() float DamagePercentage);
	UFUNCTION()
	virtual  void ChangeToAttackMode();
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateCardDisplayer(UPARAM() ECardType NewCardType);
	
public:

	ADefaultPlayerBattleModeCpp* Target;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stuff For Designers")
	FInfoStruct EnemyInfo;
	
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> Wb_BattleTurnEnemyHpBar;

	UPROPERTY(EditAnywhere, Category = "Required Objects")
	TSubclassOf<AActorRotator> ActorRotatorClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UHealthComponent> HealthComponent;

	UFUNCTION(BlueprintImplementableEvent)
	void SetReferenceAndCastUiInBP();
	UFUNCTION(BlueprintImplementableEvent)
	void SetUpBattleTurnHPBar();
	
	void SetEnemyStats(FInfoStruct Info);

	void ActivateEnemy(ADefaultPlayerBattleModeCpp* TheTarget);

	void StrafeEnded();
	
	void AttackPlayer();
	void AttackChar();
	void EndAttack();
	bool CanEnemyAttack();

	void DoCardAttack();
};