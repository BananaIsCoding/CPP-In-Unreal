// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseTurret.h"

#include "BaseProjectile.h"
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ABaseTurret::ABaseTurret()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BaseMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("BaseMeshComponent");
	SetRootComponent(BaseMeshComponent);

	BarrelMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("BarrelMeshComponent");
	BarrelMeshComponent->SetupAttachment(BaseMeshComponent);
	BarrelMeshComponent->SetRelativeLocation(FVector(60.0f, 0.0f, 0.0f));
	BarrelMeshComponent->SetRelativeScale3D(FVector(0.5f));

	FirePoint = CreateDefaultSubobject<UArrowComponent>(TEXT("FirePoint"));
	FirePoint->SetupAttachment(BarrelMeshComponent);
	FirePoint->SetRelativeLocation(FVector(280.0f, 0.0f, 0.0f));
}

// Called when the game starts or when spawned
void ABaseTurret::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(FireTimer, this, &ABaseTurret::Fire, FireSpeed, true);
}

// Called every frame
void ABaseTurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseTurret::Fire_Implementation()
{
	const FVector Location = FirePoint->GetComponentLocation();
	const FRotator Rotation = FirePoint->GetComponentRotation();

	GetWorld()->SpawnActor(ProjectileClass, &Location, &Rotation);

	if (FireSound)
		UGameplayStatics::PlaySound2D(GetWorld(), FireSound, FireSoundVolume);
}

