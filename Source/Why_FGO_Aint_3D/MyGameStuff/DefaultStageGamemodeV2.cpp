// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultStageGamemodeV2.h"
#include "StandardFgoDamageType.h"
#include "DefaultPlayerBattleModeCpp.h"
#include "FgoDefaultGameInstance.h"
#include "IntroCameraCpp.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Structs/CardPoolItem.h"
#include "Widgets/TestHpWidget.h"

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
		TheWorld->SpawnActor(IntroCamClass, &CamSpawnTransform, SpawnParameters)
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

		int enemyIndexToStartAt = 0;
		int enemyIndexToEndAt = SplitCountForChar;

		for (int i = 0; i < playerCount; i++)
		{
			for (int enemyIndex = enemyIndexToStartAt; enemyIndex < enemyIndexToEndAt; enemyIndex++)
			{
				EnemiesArray[enemyIndex]->ActivateEnemy(PartyArray[i]);
				PartyArray[i]->AddEnemyToManager(EnemiesArray[enemyIndex]);
			}
			enemyIndexToStartAt += SplitCountForChar;
			enemyIndexToEndAt += SplitCountForChar; 
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

void ADefaultStageGamemodeV2::ViewEnemy()
{
	ABaseBattleEnemyCpp* EnemyBeingViewed =  EnemiesArray[ViewingEnemyIndex];
	FVector NewLocation = EnemyBeingViewed->GetActorLocation();
	FRotator NewRotation =  EnemyBeingViewed->GetActorRotation();
	FVector ForwardVector = UKismetMathLibrary::GetForwardVector(NewRotation);
	ForwardVector.Normalize(0.0001);
	NewLocation += ForwardVector * FVector( 400.0f, 400.0f, 0.0f);
	NewRotation.Yaw += 180;
	FActorSpawnParameters SpawnParameters;
	CardTurnCamera->SetActorLocation(NewLocation);
	CardTurnCamera->SetActorRotation(NewRotation);
	UpdateEnemyViewUI( EnemyBeingViewed->HealthComponent->CurrentHealth,EnemyBeingViewed->EnemyInfo);
	EnemyBeingViewed->SetActorHiddenInGame(false);
}

void ADefaultStageGamemodeV2::BeginPlay()
{
	Super::BeginPlay();
	TheWorld = GetWorld();
	WorldTimerManager = &TheWorld->GetTimerManager();
	PlayerController = TheWorld->GetFirstPlayerController();
	FreeRoamCharacter = Cast<ADefaultCharacter>(PlayerController->GetCharacter());
	
	PartyMenu = CreateWidget(PlayerController, Wb_PartyMenu);
	MainHpBar = CreateWidget(PlayerController, Wb_MainHpBarUI);
	EnemyBattleHpList = CreateWidget(PlayerController, Wb_EnemyBattleHpListUI);
	CardSelectionList = CreateWidget(PlayerController, Wb_CardSelectionUI);
	EnemySelectionMenu = CreateWidget(PlayerController, Wb_EnemySelectionMenuUI);
	CppMainHpBar = Cast<UTestHpWidget>(CreateWidget(PlayerController, Wb_MainHpBarUiCpp));

	FActorSpawnParameters SpawnParameters;
	FTransform SpawnTransform = FTransform(FRotator::ZeroRotator, FVector(0.0f,0.0f,1000.0f), FVector::OneVector);

	CardTurnCamera = Cast<AIntroCameraCpp>(
		TheWorld->SpawnActor(IntroCamClass, &SpawnTransform, SpawnParameters)
	);
	
	int PlayerIndex = 0;
	for (TSubclassOf<ADefaultPlayerBattleModeCpp> CharBP : Cast<UFgoDefaultGameInstance>(TheWorld->GetGameInstance())->PartyCharRefArray)
	{
		ADefaultPlayerBattleModeCpp* Character = Cast<ADefaultPlayerBattleModeCpp>(TheWorld->
			SpawnActor(
				CharBP,
				&SpawnTransform,
				SpawnParameters
			)
		);
		
	 	Character->DisableCharacter();
	 	PartyArray.Add(Character);
	 	
	 	for(int i = 0; i < 5; i++)
	 	{
	 		FCardPoolItem Item = {PlayerIndex, i};
	 		CardPool.Add(Item);
	 	}
	 	PlayerIndex++;
	}

	SetReferenceAndCastUiInBP();
}

void ADefaultStageGamemodeV2::AddEnemyToBattle(ABaseEnemyCpp* EnemyToAdd)
{
	EnemiesToAdd.Add(EnemyToAdd);
	EnemyToAdd->Destroy();
}

void ADefaultStageGamemodeV2::ChangeCharPossess(ADefaultPlayerBattleModeCpp* PlayerCharacter)
{
	if (PlayerCharacter != CurrentPossessedChar)
	{
		CurrentPossessedChar->OpenPartyMenu();
		CurrentPossessedChar->ControlledByPlayer = false;
		PlayerController->Possess(PlayerCharacter);
		CurrentPossessedChar = PlayerCharacter;
		CurrentPossessedChar->ControlledByPlayer = true;
		CurrentPossessedChar->HealthComponent->TakeDamage(0);
	}
}

void ADefaultStageGamemodeV2::ChangeEnemyView(bool GoBackward)
{
	EnemiesArray[ViewingEnemyIndex]->SetActorHiddenInGame(true);
	if (GoBackward)
	{
		if (ViewingEnemyIndex != 0)
		{
			ViewingEnemyIndex--;
		}
		else
		{
			ViewingEnemyIndex = EnemiesArray.Num() - 1;
		}
	}
	else
	{
		if (ViewingEnemyIndex < EnemiesArray.Num() - 1)
		{
			ViewingEnemyIndex++;
		}
		else
		{
			ViewingEnemyIndex = 0;
		}
	}
	ViewEnemy();
}

void ADefaultStageGamemodeV2::ChosenCard(int IndexInPoolArray)
{
	CardSelectionList->RemoveFromParent();
	ADefaultPlayerBattleModeCpp* PlayerChar = PartyArray[CardPool[IndexInPoolArray].CharIndex];
	int PlayerCardIndex = CardPool[IndexInPoolArray].SkillIndex;
	if (PlayerAttackCount > 2)
	{
		PlayerChar->HealthComponent->ChosenCard = PlayerChar->CardSkillList[PlayerCardIndex];
		PlayerCardAttackCompleteStuff();
	}
	else
	{
		EnemySelectionMenu->RemoveFromParent();
		PlayerChar->DoCardAttack(PlayerCardIndex);
	}
}

void ADefaultStageGamemodeV2::PlayerCardAttackCompleteStuff()
{
	if (!EnemiesArray.IsEmpty())
	{
		PlayerAttackCount++;
		if (PlayerAttackCount > 3)
		{
			EnemiesArray[ViewingEnemyIndex]->DoCardAttack();
		}
		else
		{
			CardSelectionList->AddToViewport();
			if (PlayerAttackCount == 3)
			{
				EnemiesArray[ViewingEnemyIndex]->SetActorHiddenInGame(true);
				ViewingEnemyIndex = 0;
				ViewEnemy();
				ChangeToDefenceMode();
				if (ShowEnemyTarget.IsBound())
					ShowEnemyTarget.Broadcast();
			}
			else
			{
				EnemySelectionMenu->AddToViewport();
			}
		}
	}
}

void ADefaultStageGamemodeV2::EnemyCardAttackCompleteStuff()
{
	ViewingEnemyIndex++;
	if (ViewingEnemyIndex == EnemiesArray.Num())
	{
		BattleTurn();
	}
	else
	{
		ViewEnemy();
		if (PlayerAttackCount == 5)
		{
			FTimerHandle TimerHandle;
			WorldTimerManager->SetTimer(TimerHandle, this, &ADefaultStageGamemodeV2::DelayEnemyAttackAnim, 1.0f, false);
		}
		else
		{
			CardSelectionList->AddToViewport();
		}
	}
}

void ADefaultStageGamemodeV2::DelayEnemyAttackAnim()
{
	EnemiesArray[ViewingEnemyIndex]->DoCardAttack();
}

void ADefaultStageGamemodeV2::DealDamageToCurrentViewingEnemy(float Damage, EFgoClassType ClassType, ECardType CardType)
{
	EnemiesArray[ViewingEnemyIndex]->HealthComponent->TakeDamage(Damage, ClassType, CardType);
}

void ADefaultStageGamemodeV2::OnEnemyDefeat(ABaseBattleEnemyCpp* DefeatedEnemy)
{
	if (EnemiesArray.Num() == 1)
	{
		EnemiesArray.RemoveAt(0);
		DefeatedEnemy->Destroy();
		EndBattleMode();
	}
	else
	{
		for (int i = 0; i < EnemiesArray.Num(); i++)
		{
			if (EnemiesArray[i] == DefeatedEnemy)
			{
				if (CurrentBattleState == EBattleState::InCardTurn)
				{
					if (i == EnemiesArray.Num() - 1)
					{
						ViewingEnemyIndex--;
						ViewEnemy();
						EnemiesArray.RemoveAt(i);
					}
					else
					{
						EnemiesArray.RemoveAt(i);
						if (i == ViewingEnemyIndex)
						{
							ViewEnemy();
						}
					}
					break;
				}
				EnemiesArray.RemoveAt(i);
				break;
			}
		}
		//EnemiesArray.RemoveSingle(DefeatedEnemy);
		DefeatedEnemy->Destroy();
	}
}

void ADefaultStageGamemodeV2::OnCharacterDefeat(ADefaultPlayerBattleModeCpp* PlayerChar)
{
	if (EnemiesArray.Num() == 1)
	{
		UKismetSystemLibrary::QuitGame(TheWorld, PlayerController, EQuitPreference::Quit, true);
	}
	else
	{
		if (PartyArray.RemoveSingle(PlayerChar) == 1)
		{
			if (CurrentPossessedChar == PlayerChar)
			{
				PlayerController->Possess(PartyArray[0]);
			}
			// [NOTE] do reassign enemy
		}
	}
}

void ADefaultStageGamemodeV2::EndBattleMode()
{
	BattleZoneBp->Destroy();
	UWidgetLayoutLibrary::RemoveAllWidgets(PlayerController);
	if (OnBattleEnded.IsBound())
		OnBattleEnded.Broadcast();
	FreeRoamCharacter->EnableCharacter();
	PlayerController->Possess(FreeRoamCharacter);
	PlayerController->SetShowMouseCursor(false);
	PlayerController->SetInputMode(FInputModeGameOnly());
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
		ABaseBattleEnemyCpp* NewBattleEnemy = Cast<ABaseBattleEnemyCpp>(TheWorld->
			SpawnActor(
				Enemy->BattleEnemyClass,
				&EnemySpawnTransform,
				SpawnParameters
			)
		);

		NewBattleEnemy->SetEnemyStats(Enemy->EnemyInfo);
		
		EnemiesArray.Add(NewBattleEnemy);
		Index += 1;
	}
	
	SetUpIntroCamera(FRotator::ZeroRotator, 200.0f, TempArray.Num());
	TempArray.Empty();
}

void ADefaultStageGamemodeV2::IntroPlayer()
{
	FreeRoamCharacter->DisableCharacter();
	
	CalcOffset(PartyArray.Num());

	int Index = 0;
	for (ADefaultPlayerBattleModeCpp* Character : PartyArray)
	{
		Character->EnableCharacter();
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
	WorldTimerManager->SetTimer(TimerHandle, this, &ADefaultStageGamemodeV2::BattleTurn, WaitTime, false);
}

void ADefaultStageGamemodeV2::BattleTurn()
{
	CurrentBattleState = EBattleState::InBattleTurn;
	if (EnemiesToAdd.IsEmpty())
	{
		PlayerController->Possess(CurrentPossessedChar);
		PlayerController->SetShowMouseCursor(false);
		PlayerController->SetInputMode(FInputModeGameOnly());
		
		//MainHpBar->AddToViewport();
		CppMainHpBar->AddToViewport();
		EnemyBattleHpList->AddToViewport();

		EnemyTargetPicker();
	}
	else
	{
		// remove main hp bar
		IntroCutsceneWaiter(EnemiesToAdd.Num());
		IntroEnemy();
	}
	FTimerHandle TimerHandle;
	WorldTimerManager->SetTimer(TimerHandle, this, &ADefaultStageGamemodeV2::CardTurnSetUp, BattleTurnDuration, false);
}

void ADefaultStageGamemodeV2::CardTurnSetUp()
{
	CurrentBattleState = EBattleState::InCardTurn;
	EnemyBattleHpList->RemoveFromParent();
	if (OnCardPrepTurn.IsBound())
		OnCardPrepTurn.Broadcast();
	CardPoolInUse = {0, 1, 2, 3, 4};
	if (PartyArray.Num() != 1)
	{
		// this array will be used to pick a random index of the actual pool with no repeat picks
		TArray<int> CardsToPickFrom;
		for (int i = 0; i < CardPool.Num(); i++)
		{
			CardsToPickFrom.Add(i);
		}
		for (int J = 0; J < 5; J++)
		{
			int RandIndex = FMath::RandRange(0, CardsToPickFrom.Num() - 1);
			CardPoolInUse[J] = CardsToPickFrom[RandIndex];
			CardsToPickFrom.RemoveAt(RandIndex);
		}
	}
	CardSelectionList->AddToViewport();
	EnemySelectionMenu->AddToViewport();
	PlayerController->SetShowMouseCursor(true);
	PlayerController->SetInputMode(FInputModeUIOnly());
	PlayerController->FlushPressedKeys();
	// give time to stop enemy moving
	FTimerHandle TimerHandle;
	ViewingEnemyIndex = 0;
	ViewEnemy();
	PlayerController->SetViewTargetWithBlend(CardTurnCamera, 1.0f);
	CLearCardSelection();
	PlayerAttackCount = 0;
	for (int Num = 0; Num < 5; Num++)
	{
		FCardPoolItem CardInfo = CardPool[CardPoolInUse[Num]];
		ADefaultPlayerBattleModeCpp* Player = PartyArray[CardInfo.CharIndex];
		PassCardInfo(
			Player->PlayerInfo.Name,
			Player->CardSkillList[CardInfo.SkillIndex],
			CardPoolInUse[Num],
			Num
		);
	}
}

void ADefaultStageGamemodeV2::BattleSetUp(FTransform BattleStartPos)
{
	CurrentBattleState = EBattleState::SettingUp;
	EngagePos = BattleStartPos;
	FActorSpawnParameters SpawnParameters;
	BattleZoneBp = Cast<ABattleZoneCpp>(TheWorld->SpawnActor(BattleZoneClass,&EngagePos, SpawnParameters));
}

void ADefaultStageGamemodeV2::PreBattleIntro()
{
	IntroEnemy();

	FTimerHandle TimerHandle;
	float WaitTime = EnemiesArray.Num() * IntroCutsceneMultiplier;
	WaitTime = 0.1f;
	WorldTimerManager->SetTimer(TimerHandle, this, &ADefaultStageGamemodeV2::IntroPlayer, WaitTime, false);
}