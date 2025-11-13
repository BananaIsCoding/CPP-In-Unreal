// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Why_FGO_Aint_3D/MyGameStuff/Structs/InfoStruct.h"
#include "BaseBattleEnemyCpp.generated.h"

struct FAIRequestID;
class UBoxComponent;
namespace EPathFollowingResult
{
	enum Type : int;
}
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
	
	UPROPERTY(EditAnywhere)
	class UTextRenderComponent* TextComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UHealthComponent> HealthComponent;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USphereComponent> AttackRangeHitBox;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UWidgetComponent> CardInfoSceneWidget;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> AttackHitBox;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI")
	class UUserWidget* BattleModeHPBar;

	ADefaultPlayerBattleModeCpp* Target;

	EEnemyState CurrentState = CardTurnMode;

	bool IsPlayerInRange = false;

	AActorRotator* RotatePoint;

	FTimerHandle BehaviorLoopTimerHandler;
	FTimerHandle LookTimerHandler;

	bool IsStrafeEnded = true;
	bool IsHitStun = false;

	FVector BeforeAttackPos;
	bool TargetIsNotNpc;

	UFUNCTION()
	void OnAttackRangeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnAttackingOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void BeginPlay() override;
	
	void StartBehaviourLoop();

	void BehaviourLoop();

	void LookAtPlayer();

	void MoveToAttackPos();

	UFUNCTION()
	void CheckOnAttackPos(FAIRequestID RequestID, EPathFollowingResult::Type Result);
	
public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stuff For Designers")
	FInfoStruct EnemyInfo;
	
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> Wb_BattleTurnEnemyHpBar;

	UPROPERTY(EditAnywhere, Category = "Required Objects")
	TSubclassOf<AActorRotator> ActorRotatorClass;

	void SetEnemyStats(FInfoStruct Info);

	void ActivateEnemy(ADefaultPlayerBattleModeCpp* TheTarget);

	void StrafeEnded();
	
	UFUNCTION(BlueprintImplementableEvent)
	void SetUpBattleTurnHPBar();

	void AttackPlayer();

	void AttackChar();

	void EndAttack();

	bool CanEnemyAttack();
};
