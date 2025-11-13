// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultPlayerBattleModeCpp.h"
#include "DefaultStageGamemodeV2.h"
#include "AI/NavigationSystemBase.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextRenderComponent.h"


// Sets default values
ADefaultPlayerBattleModeCpp::ADefaultPlayerBattleModeCpp()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TextComponent = CreateDefaultSubobject<UTextRenderComponent>("NameTag");
	TextComponent->SetupAttachment(RootComponent);
	TextComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	TextComponent->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
}

void ADefaultPlayerBattleModeCpp::BeginPlay()
{
	Super::BeginPlay();

	TextComponent->SetText(FText::FromName(PlayerInfo.Name));

	DefaultStageGamemode = Cast<ADefaultStageGamemodeV2>(GetWorld()->GetAuthGameMode());
	
	HealthComponent->MaxHealth = PlayerInfo.Health;
	HealthComponent->CurrentHealth = PlayerInfo.Health;

	PartyMenuHPBar = CreateWidget(GetWorld()->GetGameInstance(), Wb_PartyMenuHpItem);

	SetUpPartyMenuHPBar();
}

