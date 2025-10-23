// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuGamemode.h"
#include "Blueprint/UserWidget.h"


AMainMenuGamemode::AMainMenuGamemode()
{
}

void AMainMenuGamemode::BeginPlay()
{
	Super::BeginPlay();

	SetUpUI();

	MainMenuUI->AddToViewport();
}

void AMainMenuGamemode::SetUpUI()
{
	if (IsValid(Wb_MainMenu))
	{
		MainMenuUI = CreateWidget(GetWorld(), Wb_MainMenu);
	}
	if (IsValid(Wb_StageSelect))
	{
		StageSelectUI = CreateWidget(GetWorld(), Wb_StageSelect);
	}
}

void AMainMenuGamemode::DisplayStageSelectUI_Implementation()
{
	MainMenuUI->RemoveFromParent();
	StageSelectUI->AddToViewport();
}

void AMainMenuGamemode::DisplayMainMenuUI_Implementation()
{
	StageSelectUI->RemoveFromParent();
	MainMenuUI->AddToViewport();
}
