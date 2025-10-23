// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MainMenuGamemode.generated.h"

/**
 * 
 */
UCLASS()
class WHY_FGO_AINT_3D_API AMainMenuGamemode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMainMenuGamemode();

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> Wb_MainMenu;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> Wb_StageSelect;
	
	UPROPERTY(EditAnywhere)
	class UUserWidget* MainMenuUI;
	UPROPERTY(EditAnywhere)
	class UUserWidget* StageSelectUI;
	
	virtual void BeginPlay() override;
	
	void SetUpUI();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MyEvents")
	void DisplayStageSelectUI();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MyEvents")
	void DisplayMainMenuUI();
};
