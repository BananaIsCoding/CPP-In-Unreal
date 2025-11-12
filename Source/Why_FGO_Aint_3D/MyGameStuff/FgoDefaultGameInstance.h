// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "FgoDefaultGameInstance.generated.h"

class ADefaultPlayerBattleModeCpp;
/**
 * 
 */
UCLASS()
class WHY_FGO_AINT_3D_API UFgoDefaultGameInstance : public UGameInstance
{
	GENERATED_BODY()

	UFgoDefaultGameInstance();

public:
	UPROPERTY(EditAnywhere)
	//TArray<ADefaultPlayerBattleModeCpp*> PartyCharRefArray;
	TArray<TSubclassOf<ADefaultPlayerBattleModeCpp>> PartyCharRefArray;
	
};
