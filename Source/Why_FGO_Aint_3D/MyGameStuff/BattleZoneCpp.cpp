// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleZoneCpp.h"
#include "DefaultStageGamemodeV2.h"
#include "Components/SphereComponent.h"

// Sets default values
ABattleZoneCpp::ABattleZoneCpp()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereCollider->SetCollisionProfileName("OnlyDetectEnemy");
	SphereCollider->SetSphereRadius(3000.0f);
	SphereCollider->OnComponentBeginOverlap.AddUniqueDynamic(this, &ABattleZoneCpp::OnHitBoxOverlay);
}

// Called when the game starts or when spawned
void ABattleZoneCpp::BeginPlay()
{
	Super::BeginPlay();
	DefaultStageGamemode = (ADefaultStageGamemodeV2*) GetWorld()->GetAuthGameMode();
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ABattleZoneCpp::StartIntro, 1.0f, false);
}

void ABattleZoneCpp::StartIntro()
{
	DefaultStageGamemode->PreBattleIntro();
}

void ABattleZoneCpp::OnHitBoxOverlay(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABaseEnemyCpp* Enemy = Cast<ABaseEnemyCpp>(OtherActor);
	DefaultStageGamemode->AddEnemyToBattle(Enemy);
}

