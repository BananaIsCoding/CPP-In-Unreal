// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TestHpWidget.generated.h"

/**
 * 
 */
UCLASS()
class WHY_FGO_AINT_3D_API UTestHpWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly)
	float HealthPercentage;
	UPROPERTY(BlueprintReadOnly)
	FText HealthText;
	

public:

	virtual void UpdateHealthBar(float CurrentHp, float MaxHp);
	
};
