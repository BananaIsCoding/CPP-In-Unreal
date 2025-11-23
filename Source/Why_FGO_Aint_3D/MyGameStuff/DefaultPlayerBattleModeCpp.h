// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Structs/InfoStruct.h"
#include "Why_FGO_Aint_3D/DefaultCharacter.h"
#include "DefaultPlayerBattleModeCpp.generated.h"

class ABaseBattleEnemyCpp;
class UTextRenderComponent;
class ADefaultStageGamemodeV2;
class UUserWidget;

UCLASS()
class WHY_FGO_AINT_3D_API ADefaultPlayerBattleModeCpp : public ADefaultCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADefaultPlayerBattleModeCpp();

protected:

	// to stop code searching for key things every time it needs it
	FTimerManager* WorldTimerManager;
	APlayerController* PlayerController;
	
	UPROPERTY(EditAnywhere)
	class UTextRenderComponent* TextComponent;
	
	bool MenuOpened = false;

	int CurrentEnemyIndex = 0;

	FTimerHandle ManagerTimerHandle;
	FTimerHandle WaitTimerHandle;

	int ChosenCardIndex;
	
	virtual void BeginPlay() override;

	UFUNCTION()
	void UpdateHealthUI();

	void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void AttackFunction_Implementation() override;
	void EndAttack();
	virtual void TurnAttackCooldownOff() override;

	UFUNCTION()
	void OnAttackHitBoxOverlay(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnCharDeath();
	UFUNCTION()
	void PrepareForCardTurn();

	UFUNCTION(BlueprintImplementableEvent)
	void CardAttack1Stuff();
	UFUNCTION(BlueprintImplementableEvent)
	void CardAttack2Stuff();
	UFUNCTION(BlueprintImplementableEvent)
	void CardAttack3Stuff();
	UFUNCTION(BlueprintImplementableEvent)
	void CardAttack4Stuff();
	UFUNCTION(BlueprintImplementableEvent)
	void CardAttack5Stuff();
	UFUNCTION(BlueprintCallable)
	void EndCardAttack(UPARAM() float AttackAnimDuration, UPARAM() bool DoYouNeedMeToDoDmg);
	UFUNCTION(BlueprintCallable)
	void DealCardDamage(UPARAM() float DamagePercentage);

public:

	TArray<ABaseBattleEnemyCpp*> EnemyArray;
	
	bool ControlledByPlayer = false;
	
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* TabPressedAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stuff For Designers")
	FInfoStruct PlayerInfo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stuff For Designers")
	TArray<ECardType> CardSkillList;
	

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> Wb_PartyMenuHpItem;

	UPROPERTY(BlueprintReadOnly, Category = "C++ Public Variables" )
	ADefaultStageGamemodeV2* DefaultStageGamemode;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	class UUserWidget* PartyMenuHPBar;

	void DisableCharacter() override;
	void EnableCharacter() override;
	
	UFUNCTION(BlueprintImplementableEvent)
	void SetReferenceAndCastUiInBP();
	UFUNCTION(BlueprintImplementableEvent)
	void AddHpBarToList();
	UFUNCTION(BlueprintImplementableEvent)
	void UpdatePartyMenuHpUI();
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateMainHpUI();
	
	void AddEnemyToManager(ABaseBattleEnemyCpp* Enemy);
	void StartEnemyManager(bool IsControlledByPlayer);
	void MakeEnemyAttack();
	void StopEnemyManager();

	void OpenPartyMenu();

	void DoCardAttack(int CardNumber);
};
