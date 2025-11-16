// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enums/FgoClassTypeEnum.h"
#include "GameFramework/DamageType.h"
#include "StandardFgoDamageType.generated.h"

/**
 * 
 */
UCLASS()
class WHY_FGO_AINT_3D_API UStandardFgoDamageType : public UDamageType
{

	GENERATED_BODY()
public:
	
	void Init(EFgoClassType FgoClass);

protected:

	float CalcClassAdvantage(EFgoClassType ClassOfDefender);
public:

	float CalcDamageTaken(float BaseDamage, EFgoClassType ClassOfDefender);

	EFgoClassType ClassOfAttacker;
	
};
