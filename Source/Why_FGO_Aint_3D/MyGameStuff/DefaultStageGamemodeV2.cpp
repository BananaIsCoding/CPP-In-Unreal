// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultStageGamemodeV2.h"

#include "DefaultPlayerBattleModeCpp.h"
#include "FgoDefaultGameInstance.h"
#include "IntroCameraCpp.h"
#include "Blueprint/UserWidget.h"
#include "Structs/CardPoolItem.h"

ADefaultStageGamemodeV2::ADefaultStageGamemodeV2()
{
}

void ADefaultStageGamemodeV2::CalcOffset(int ArrayLen)
{
	if (ArrayLen == 1)
	{
		PosOffset = 0;
	}
	else
	{
		PosOffset = (ArrayLen / 2) * (SpawnSpacing * -1);
		if (ArrayLen % 2 == 0)
		{
			PosOffset += SpawnSpacing / 2;
		}
	}
	//UE_LOG(LogTemp, Log, TEXT("Example text that prints a float: %f"), PosOffset);
}

FTransform ADefaultStageGamemodeV2::GetSpawnPosition(FRotator Rotation, float XSpawnOffset, int Index)
{
	FVector NewLocation = FVector(EngagePos.GetLocation().X + XSpawnOffset,(SpawnSpacing * Index) + PosOffset + EngagePos.GetLocation().Y, EngagePos.GetLocation().Z);

	return FTransform(Rotation.Quaternion(), NewLocation, FVector::One());
}

void ADefaultStageGamemodeV2::SetUpIntroCamera(FRotator Rotation, float XSpawnOffset, int Index)
{
	FActorSpawnParameters SpawnParameters;
	
	FTransform CamSpawnTransform = FTransform(Rotation, FVector(EngagePos.GetLocation().X + XSpawnOffset, EngagePos.GetLocation().Y, EngagePos.GetLocation().Z));
	Cast<AIntroCameraCpp>(
		GetWorld()->SpawnActor(IntroCamClass, &CamSpawnTransform, SpawnParameters)
	)->StartTheCutscene((SpawnSpacing * Index) + 100.0f );
	
}

void ADefaultStageGamemodeV2::EnemyTargetPicker()
{
	int playerCount = PartyArray.Num();
	int enemyCount = EnemiesArray.Num();
	if (enemyCount == 1)
	{
		EnemiesArray[0]->ActivateEnemy(CurrentPossessedChar);
		PartyArray[0]->AddEnemyToManager(EnemiesArray[0]);
	}
	else
	{
		int SplitCountForChar = enemyCount / playerCount;
		UE_LOG(LogTemp, Log, TEXT("Split: %i"), SplitCountForChar);

		int enemyIndexToStartAt = 0;
		int enemyIndexToEndAt = SplitCountForChar - 1;
		
		for (int i = 0; i < playerCount; i++)
		{
			for (int enemyIndex = enemyIndexToStartAt; enemyIndex < enemyIndexToEndAt; enemyIndex++)
			{
				EnemiesArray[enemyIndex]->ActivateEnemy(PartyArray[i]);
				PartyArray[i]->AddEnemyToManager(EnemiesArray[enemyIndex]);
			}
			enemyIndexToStartAt += SplitCountForChar - 1;
			enemyIndexToEndAt += SplitCountForChar - 1; 
		}

		if (enemyCount % playerCount != 0)
		{
			for (int i = SplitCountForChar * playerCount; i < enemyCount; i++)
			{
				EnemiesArray[i]->ActivateEnemy(CurrentPossessedChar);
				PartyArray[0]->AddEnemyToManager(EnemiesArray[i]);
			}
		}
	}
	
	for (ADefaultPlayerBattleModeCpp* Player : PartyArray)
	{
		Player->StartEnemyManager(Player == CurrentPossessedChar);
		Player->SetActorHiddenInGame(false);
	}
}

void ADefaultStageGamemodeV2::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	PartyMenu = CreateWidget(PlayerController, Wb_PartyMenu);
	MainHpBar = CreateWidget(PlayerController, Wb_MainHpBarUI);
	EnemyBattleHpList = CreateWidget(PlayerController, Wb_EnemyBattleHpListUI);
	
	FActorSpawnParameters SpawnParameters;
	FTransform SpawnTransform = FTransform(FRotator::ZeroRotator, FVector(0.0f,0.0f,1000.0f), FVector::OneVector);
	
	int PlayerIndex = 0;
	
	for (TSubclassOf<ADefaultPlayerBattleModeCpp> CharBP : Cast<UFgoDefaultGameInstance>(GetWorld()->GetGameInstance())->PartyCharRefArray)
	{
		ADefaultPlayerBattleModeCpp* Character = Cast<ADefaultPlayerBattleModeCpp>(GetWorld()->
			SpawnActor(
				CharBP,
				&SpawnTransform,
				SpawnParameters
			)
		);
		
	 	Character->SetActorHiddenInGame(true);
	 	PartyArray.Add(Character);
	 	
	 	for(int i = 0; i < 4; i++)
	 	{
	 		FCardPoolItem Item = {PlayerIndex, i};
	 		CardPool.Add(Item);
	 	}
	 	PlayerIndex++;
	}
}

void ADefaultStageGamemodeV2::AddEnemyToBattle(ABaseEnemyCpp* EnemyToAdd)
{
	EnemiesToAdd.Add(EnemyToAdd);
	EnemyToAdd->Destroy();
}

void ADefaultStageGamemodeV2::IntroEnemy()
{
	TArray<ABaseEnemyCpp*> TempArray = EnemiesToAdd;
	EnemiesToAdd.Empty();
	CalcOffset(TempArray.Num());
	
	FActorSpawnParameters SpawnParameters;
	int Index = 0;
	for (ABaseEnemyCpp* Enemy : TempArray)
	{
		FTransform EnemySpawnTransform = GetSpawnPosition(FRotator(0, 180, 0),500.0f, Index);
		ABaseBattleEnemyCpp* NewBattleEnemy = Cast<ABaseBattleEnemyCpp>(GetWorld()->
			SpawnActor(
				Enemy->BattleEnemyClass,
				&EnemySpawnTransform,
				SpawnParameters
			)
		);
		
		EnemiesArray.Add(NewBattleEnemy);
		Index += 1;
	}
	
	SetUpIntroCamera(FRotator::ZeroRotator, 200.0f, TempArray.Num());
	TempArray.Empty();
}

void ADefaultStageGamemodeV2::IntroPlayer()
{
	UE_LOG(LogTemp, Log, TEXT("Run"));
	GetWorld()->GetFirstPlayerController()->GetCharacter()->Destroy();
	CalcOffset(PartyArray.Num());

	int Index = 0;
	for (ADefaultPlayerBattleModeCpp* Character : PartyArray)
	{
		Character->SetActorHiddenInGame(false);
		FTransform SpawnTransform = GetSpawnPosition(FRotator::ZeroRotator,-500.0f, Index);
		Character->SetActorTransform(SpawnTransform);
		Index++;
	}

	SetUpIntroCamera(FRotator(0.0f, 180.0f, 0.0f), -200.0f, PartyArray.Num());

	CurrentPossessedChar = PartyArray[0];

	IntroCutsceneWaiter(PartyArray.Num());
}

void ADefaultStageGamemodeV2::IntroCutsceneWaiter(int Num)
{
	FTimerHandle TimerHandle;
	float WaitTime = Num * IntroCutsceneMultiplier;
	WaitTime = .1f;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ADefaultStageGamemodeV2::BattleTurn, WaitTime, false);
}

void ADefaultStageGamemodeV2::BattleTurn()
{
	if (EnemiesToAdd.IsEmpty())
	{
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		PlayerController->Possess(CurrentPossessedChar);
		PlayerController->SetShowMouseCursor(false);
		PlayerController->SetInputMode(FInputModeGameOnly());
		
		MainHpBar->AddToViewport();
		EnemyBattleHpList->AddToViewport();

		EnemyTargetPicker();
	}
	else
	{
		// remove main hp bar
		IntroCutsceneWaiter(EnemiesToAdd.Num());
		IntroEnemy();
	}
}

void ADefaultStageGamemodeV2::BattleSetUp(FTransform BattleStartPos)
{
	EngagePos = BattleStartPos;
	FActorSpawnParameters SpawnParameters;
	BattleZoneBp = Cast<ABattleZoneCpp>(GetWorld()->SpawnActor(BattleZoneClass,&EngagePos, SpawnParameters));
}

void ADefaultStageGamemodeV2::PreBattleIntro()
{
	IntroEnemy();

	FTimerHandle TimerHandle;
	float WaitTime = EnemiesArray.Num() * IntroCutsceneMultiplier;
	WaitTime = 0.1f;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ADefaultStageGamemodeV2::IntroPlayer, WaitTime, false);
}
