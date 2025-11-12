// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseBattleEnemyCpp.h"

#include "Blueprint/UserWidget.h"
#include "TutorialStuff/HealthComponent.h"
#include "Components/TextRenderComponent.h"


// Sets default values
ABaseBattleEnemyCpp::ABaseBattleEnemyCpp()
{
	PrimaryActorTick.bCanEverTick = true;
	
	/*SceneComponent = CreateDefaultSubobject<USceneComponent>("SceneComponent");
	RootComponent = SceneComponent;*/
	TextComponent = CreateDefaultSubobject<UTextRenderComponent>("NameTag");
	TextComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	TextComponent->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
	
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
}

void ABaseBattleEnemyCpp::SetEnemyStats(FInfoStruct Info)
{
	EnemyInfo = Info;
	FString Nametag = EnemyInfo.Name.ToString() + "(" + EnemyInfo.Class.ToString() + ")";
	
	TextComponent->SetText(FText::FromString(Nametag));
	HealthComponent->MaxHealth = EnemyInfo.Health;
	HealthComponent->CurrentHealth = EnemyInfo.Health;

	BattleModeHPBar = CreateWidget(GetWorld()->GetGameInstance(), Wb_BattleTurnEnemyHpBar);
	
	SetUpBattleTurnHPBar();
}

