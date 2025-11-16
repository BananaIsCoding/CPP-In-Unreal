// Fill out your copyright notice in the Description page of Project Settings.
#include "TestHpWidget.h"

void UTestHpWidget::UpdateHealthBar(float CurrentHp, float MaxHp)
{
	HealthPercentage = CurrentHp / MaxHp;
	FString NewHealthString = "Health: " + FString::SanitizeFloat(CurrentHp) + "/" + FString::SanitizeFloat(MaxHp);
	HealthText = FText::FromString(NewHealthString);
}
