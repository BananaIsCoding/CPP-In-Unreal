// Fill out your copyright notice in the Description page of Project Settings.
#include "DefaultStageGamemode.h"

#include "TutorialStuff/Checkpoint.h"

ADefaultStageGamemode::ADefaultStageGamemode()
{
	
}

void ADefaultStageGamemode::RegisterCheckpoint(ACheckpoint* CheckPointActor)
{
	CheckPointActor->OnEndGameDelegate.AddDynamic(this, &ADefaultStageGamemode::FinishGame);
}

void ADefaultStageGamemode::FinishGame(FString EndMessage)
{
	UE_LOG(LogTemp, Log, TEXT("%s"), *EndMessage);
}