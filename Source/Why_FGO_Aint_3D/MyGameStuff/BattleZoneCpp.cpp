// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleZoneCpp.h"
#include "Components/SphereComponent.h"

// Sets default values
ABattleZoneCpp::ABattleZoneCpp()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereCollider->SetCollisionProfileName("OverlapAllDynamic");
	SphereCollider->SetSphereRadius(3000.0f);
	
}

// Called when the game starts or when spawned
void ABattleZoneCpp::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABattleZoneCpp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

