// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseTurret.generated.h"

class ABaseProjectile;
class UArrowComponent;

UCLASS()
class WHY_FGO_AINT_3D_API ABaseTurret : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABaseTurret();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> BaseMeshComponent;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> BarrelMeshComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UArrowComponent> FirePoint;

	UPROPERTY(EditDefaultsOnly, Category= "TurretData")
	float FireSpeed = 0.5f;

	UPROPERTY(EditInstanceOnly, Category= "TurretData")
	FTimerHandle FireTimer;

	UPROPERTY(EditDefaultsOnly, Category= "TurretData")
	TSubclassOf<ABaseProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category= "TurretData")
	TObjectPtr<USoundBase> FireSound;

	UPROPERTY(EditDefaultsOnly, Category= "TurretData")
	float FireSoundVolume = 1.0f;

	/*UPROPERTY(EditDefaultsOnly, BlueprintCallable , Category= "TurretData")
	TObjectPtr<UNiagaraSystem> Fire*/

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TurretData")
	void Fire();
};
