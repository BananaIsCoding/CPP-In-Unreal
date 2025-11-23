// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultStageGamemodeV2.h"
#include "StandardFgoDamageType.h"
#include "DefaultPlayerBattleModeCpp.h"
#include "FgoDefaultGameInstance.h"
#include "IntroCameraCpp.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Structs/CardPoolItem.h"
#include "Widgets/TestHpWidget.h"

ADefaultStageGamemodeV2::ADefaultStageGamemodeV2()
{
	SceneComponent = CreateDefaultSubobject<USceneComponent>("SceneComponent");
	SetRootComponent(SceneComponent);
	AudioComponent = CreateDefaultSubobject<UAudioComponent>("AudioComponent");
	AudioComponent->SetupAttachment(RootComponent);
	AudioComponent->bAutoActivate = false;
}

// utilities functions
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
// makes enemy choose a player to attack during battle phrase
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
// change player camera to view the selected enemy
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
	StageCompleteMenu = CreateWidget(PlayerController, Wb_StageCompleteUI);
	StageFailedMenu = CreateWidget(PlayerController, Wb_StageFailedUI);
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

	PlayerController->SetShowMouseCursor(false);
	PlayerController->SetInputMode(FInputModeGameOnly());


	AudioComponent->Sound = RoamBgm;
	AudioComponent->Play();
}

// a public func to save the enemy and later add to battle
void ADefaultStageGamemodeV2::AddEnemyToBattle(ABaseEnemyCpp* EnemyToAdd)
{
	EnemiesToAdd.Add(EnemyToAdd);
	EnemyToAdd->Destroy();
}
// allow player to switch to different characters in player's party
// and tell code that character is being controlled by the player 
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
// allow player to change which enemy they are viewing
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
// runs when player picks a card and determine if it is for attack or defense
void ADefaultStageGamemodeV2::ChosenCard(int IndexInPoolArray)
{
	if (CardSelectionList->IsVisible())
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
// when card attack animation is done, prep for the next attack 
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
			if (!CardSelectionList->IsVisible())
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
		// ends card phrase if all enemy has attacked
		ViewingEnemyIndex = 0;
		FTimerHandle Handle;
		WorldTimerManager->SetTimer(Handle, this, &ADefaultStageGamemodeV2::DelayBeforeBattleTurn, 0.1f, false);
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
			if (!CardSelectionList->IsVisible())
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
		// if last enemy was defeated
		EnemiesArray.RemoveAt(0);
		DefeatedEnemy->Destroy();
		// check if there is enemy waiting to join
		if (EnemiesToAdd.Num() != 0)
		{
			if (OnCardPrepTurn.IsBound())
				OnCardPrepTurn.Broadcast();
			if (BattleModeTimerHandle.IsValid())
				WorldTimerManager->ClearTimer(BattleModeTimerHandle);
			UWidgetLayoutLibrary::RemoveAllWidgets(PlayerController);
			BattleTurn();
		}
		else
		{
			EndBattleMode();
		}
		
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
	if (PartyArray.Num() == 1)
	{
		// if last character in player's party is defeated
		PlayerController->SetShowMouseCursor(true);
		PlayerController->SetInputMode(FInputModeUIOnly());
		StageFailedMenu->AddToViewport();
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.0001);
		//UKismetSystemLibrary::QuitGame(TheWorld, PlayerController, EQuitPreference::Quit, true);
	}
	else
	{
		if (PartyArray.RemoveSingle(PlayerChar) == 1)
		{
			if (CurrentPossessedChar == PlayerChar)
			{
				PlayerController->Possess(PartyArray[0]);
				CurrentPossessedChar = PartyArray[0];
				CurrentPossessedChar->ControlledByPlayer = true;
				CurrentPossessedChar->HealthComponent->TakeDamage(0);
			}
			// [NOTE] do reassign enemy
			for (ABaseBattleEnemyCpp* Enemy : PlayerChar->EnemyArray)
			{
				Enemy->Target = CurrentPossessedChar;
			}
			PlayerChar->Destroy();
		}
	}
}
void ADefaultStageGamemodeV2::EndBattleMode()
{
	CurrentBattleState = EBattleState::EndingBattle;
	BattleZoneBp->Destroy();
	if (BattleModeTimerHandle.IsValid())
		WorldTimerManager->ClearTimer(BattleModeTimerHandle);
	UWidgetLayoutLibrary::RemoveAllWidgets(PlayerController);
	if (OnBattleEnded.IsBound())
		OnBattleEnded.Broadcast();
	FreeRoamCharacter->EnableCharacter();
	PlayerController->Possess(FreeRoamCharacter);
	PlayerController->SetShowMouseCursor(false);
	PlayerController->SetInputMode(FInputModeGameOnly());
	AudioComponent->Stop();
	AudioComponent->Sound = RoamBgm;
	AudioComponent->Play();
	FTimerHandle TimerHandle;
	WorldTimerManager->SetTimer(TimerHandle, this, &ADefaultStageGamemodeV2::BattleFullyEnded, 0.1f, false);
}
void ADefaultStageGamemodeV2::OnBossDefeat()
{
	PlayerController->SetShowMouseCursor(true);
	PlayerController->SetInputMode(FInputModeUIOnly());
	StageCompleteMenu->AddToViewport();
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.0001);
}

void ADefaultStageGamemodeV2::IntroEnemy()
{
	// gets enemy that needs to be added to battle
	TArray<ABaseEnemyCpp*> TempArray = EnemiesToAdd;
	EnemiesToAdd.Empty();
	CalcOffset(TempArray.Num());
	
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	int Index = 0;
	// for each enemy that needed to be added
	for (ABaseEnemyCpp* Enemy : TempArray)
	{
		FTransform EnemySpawnTransform = GetSpawnPosition(FRotator(0, 180, 0),500.0f, Index);
		if (Enemy->BattleEnemyClass)
		{
			ABaseBattleEnemyCpp* NewBattleEnemy = Cast<ABaseBattleEnemyCpp>(TheWorld->
				SpawnActor(
					Enemy->BattleEnemyClass,
					&EnemySpawnTransform,
					SpawnParameters
				)
			);
			// just in case enemy could not spawn due to obstruction
			if (NewBattleEnemy)
			{
				NewBattleEnemy->SetEnemyStats(Enemy->EnemyInfo);
			
				EnemiesArray.Add(NewBattleEnemy);
				Index += 1;
			}
		}
	}
	
	SetUpIntroCamera(FRotator::ZeroRotator, 200.0f, TempArray.Num());
	TempArray.Empty();
}
// same as enemy but for player
void ADefaultStageGamemodeV2::IntroPlayer()
{
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
	WorldTimerManager->SetTimer(TimerHandle, this, &ADefaultStageGamemodeV2::BattleTurn, WaitTime, false);
}
// set up for battle phrase
void ADefaultStageGamemodeV2::BattleTurn()
{
	if (CurrentBattleState != EndingBattle)
	{
		CurrentBattleState = InBattleTurn;
		// check if any enemies need to be added
		if (EnemiesToAdd.IsEmpty())
		{
			PlayerController->Possess(CurrentPossessedChar);
			PlayerController->SetShowMouseCursor(false);
			PlayerController->SetInputMode(FInputModeGameOnly());
			
			//MainHpBar->AddToViewport();
			if (!MainHpBar->IsVisible())
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
		
		WorldTimerManager->SetTimer(BattleModeTimerHandle, this, &ADefaultStageGamemodeV2::CardTurnSetUp, BattleTurnDuration, false);
	}
}
void ADefaultStageGamemodeV2::CardTurnSetUp()
{
	if (CurrentBattleState != EndingBattle)
	{
		CurrentBattleState = EBattleState::InCardTurn;
		EnemyBattleHpList->RemoveFromParent();
		if (OnCardPrepTurn.IsBound())
			OnCardPrepTurn.Broadcast();
		CardPoolInUse = {0, 1, 2, 3, 4};
		// pick 5 randoms card
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
		if (!CardSelectionList->IsVisible())
			CardSelectionList->AddToViewport();
		EnemySelectionMenu->AddToViewport();
		PlayerController->SetShowMouseCursor(true);
		PlayerController->SetInputMode(FInputModeUIOnly());
		PlayerController->FlushPressedKeys();
		FTimerHandle TimerHandle;
		ViewingEnemyIndex = 0;
		ViewEnemy();
		PlayerController->SetViewTargetWithBlend(CardTurnCamera, 1.0f);
		CLearCardSelection();
		PlayerAttackCount = 0;
		// pass info over to UI
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
}

// used to ensure that no code is running when battle is ending 
void ADefaultStageGamemodeV2::BattleFullyEnded()
{
	CurrentBattleState = UnEngaged;
}
// delay to check if battle ended between the two phrase
void ADefaultStageGamemodeV2::DelayBeforeBattleTurn()
{
	BattleTurn();
}

void ADefaultStageGamemodeV2::BattleSetUp(FTransform BattleStartPos)
{
	AudioComponent->Stop();
	AudioComponent->Sound = BattleBgm;
	AudioComponent->Play();
	CurrentBattleState = EBattleState::SettingUp;
	EngagePos = BattleStartPos;
	FActorSpawnParameters SpawnParameters;
	BattleZoneBp = Cast<ABattleZoneCpp>(TheWorld->SpawnActor(BattleZoneClass,&EngagePos, SpawnParameters));
}
void ADefaultStageGamemodeV2::PreBattleIntro()
{
	FreeRoamCharacter->DisableCharacter();
	IntroEnemy();
	FTimerHandle TimerHandle;
	float WaitTime = EnemiesArray.Num() * IntroCutsceneMultiplier;
	WorldTimerManager->SetTimer(TimerHandle, this, &ADefaultStageGamemodeV2::IntroPlayer, WaitTime, false);
}