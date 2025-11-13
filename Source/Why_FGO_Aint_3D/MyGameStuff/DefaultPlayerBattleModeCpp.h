// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Structs/InfoStruct.h"
#include "Why_FGO_Aint_3D/DefaultCharacter.h"
#include "DefaultPlayerBattleModeCpp.generated.h"

class UTextRenderComponent;
class ADefaultStageGamemodeV2;
class UUserWidget;

UCLASS()
class WHY_FGO_AINT_3D_API ADefaultPlayerBattleModeCpp : public ADefaultCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADefaultPlayerBattleModeCpp();

protected:

	UPROPERTY(EditAnywhere)
	class UTextRenderComponent* TextComponent;

	

	virtual void BeginPlay() override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stuff For Designers")
	FInfoStruct PlayerInfo;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> Wb_PartyMenuHpItem;

	UPROPERTY(BlueprintReadOnly, Category = "C++ Public Variables" )
	ADefaultStageGamemodeV2* DefaultStageGamemode;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	class UUserWidget* PartyMenuHPBar;

	UFUNCTION(BlueprintImplementableEvent)
	void SetUpPartyMenuHPBar();
	
	
};
