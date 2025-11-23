// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Why_FGO_Aint_3D/MyGameStuff/Enums/FgoCardTypeEnum.h"
#include "Why_FGO_Aint_3D/MyGameStuff/Enums/FgoClassTypeEnum.h"
#include "HealthComponent.generated.h"

// Damage, Class, Art
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDamageDone);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathEvent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnDamage, float, Damage, EFgoClassType, ClassOfAttacker, ECardType, AttackCardType);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class WHY_FGO_AINT_3D_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	float CalcClassAdvantage(EFgoClassType ClassOfAttacker);
	float CalcCardAdvantage(ECardType AttackerCardType);

public:

	void TakeDamage(float Damage);
	void TakeDamage(float Damage, EFgoClassType ClassOfAttacker);
	void TakeDamage(float Damage, EFgoClassType ClassOfAttacker, ECardType AttackCardType );

	//void TakeDamage(float Damage, EFgoClassType AttackerClass);
	UFUNCTION()
	void OnDamaged(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
	void OnDamaged(float Damage);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	float MaxHealth = 100.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	float CurrentHealth = 0.0f;
	EFgoClassType ClassOfDefender;
	ECardType ChosenCard;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnDeathEvent OnDeath;
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnDamageDone OnDamageDone;
};
