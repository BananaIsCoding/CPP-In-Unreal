// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Why_FGO_Aint_3D/MyGameStuff/Structs/InfoStruct.h"
#include "BaseBattleEnemyCpp.generated.h"

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

	/*UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess="true"))
	class USceneComponent* SceneComponent;*/

	UPROPERTY(EditAnywhere)
	class UTextRenderComponent* TextComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UHealthComponent> HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI")
	class UUserWidget* BattleModeHPBar;
	
public:

	void SetEnemyStats(FInfoStruct Info);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stuff For Designers")
	FInfoStruct EnemyInfo;
	
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> Wb_BattleTurnEnemyHpBar;

	UFUNCTION(BlueprintImplementableEvent)
	void SetUpBattleTurnHPBar();
};
