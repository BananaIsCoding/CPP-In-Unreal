// Fill out your copyright notice in the Description page of Project Settings.
#include "DefaultCharacter.h"

#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
ADefaultCharacter::ADefaultCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>("Spring Arm");
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->bUsePawnControlRotation = true;
	
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	AudioComponent = CreateDefaultSubobject<UAudioComponent>("AudioComponent");
	AudioComponent->SetupAttachment(SpringArm);
	AudioComponent->SetAutoActivate(false);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

	AttackHitBox = CreateDefaultSubobject<UBoxComponent>("BasicAttackHitBox");
	AttackHitBox->SetupAttachment(RootComponent);
	AttackHitBox->SetRelativeLocation(FVector(120.0f, 0.0f, 30.0f));
	AttackHitBox->SetRelativeScale3D(FVector(2.75f, 2.5f, 2.5f));
	AttackHitBox->SetCollisionProfileName("NoCollision");
	AttackHitBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
}
// Called when the game starts or when spawned
void ADefaultCharacter::BeginPlay()
{
	Super::BeginPlay();
}
// Called every frame
void ADefaultCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
// Called to bind functionality to input
void ADefaultCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMapping, 0);
		}
	}

	if (UEnhancedInputComponent* Input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADefaultCharacter::MoveFunction);
		Input->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADefaultCharacter::LookFunction);
		Input->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ADefaultCharacter::JumpFunction);
		Input->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ADefaultCharacter::AttackFunction_Implementation);
	}
}
// for tutorial
void ADefaultCharacter::SetActorOverlap_Implementation(AActor* OverlappedActor)
{
	ITutorial_PlayerInterface::SetActorOverlap_Implementation(OverlappedActor);
	OverlappedActorRef = OverlappedActor;
}

// to turn the free roam character off during battle mode
// and on after battle ended
void ADefaultCharacter::EnableCharacter()
{
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);
}
void ADefaultCharacter::DisableCharacter()
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);
}

// for tutorial
void ADefaultCharacter::MovementEffects_Implementation()
{
	if (!AudioComponent->IsPlaying())
	{
		AudioComponent->Play();
	}
	else
	{
		AudioComponent->StopDelayed(0.1f);
	}
}

void ADefaultCharacter::MoveFunction(const FInputActionValue& InputValue)
{
	FVector2D InputVector = InputValue.Get<FVector2D>();
	if (IsValid(Controller))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, InputVector.Y);
		AddMovementInput(RightDirection, InputVector.X);
	}
}
void ADefaultCharacter::LookFunction(const FInputActionValue& InputValue)
{
	FVector2D InputVector = InputValue.Get<FVector2D>();
	if (IsValid(Controller))
	{
		AddControllerYawInput(InputVector.X);
		AddControllerPitchInput(InputVector.Y);
		
	}
}
void ADefaultCharacter::JumpFunction()
{
	ACharacter::Jump();
}

void ADefaultCharacter::TurnAttackCooldownOff()
{
	IsOnM1Cooldown = false;
}
void ADefaultCharacter::AttackFunction_Implementation()
{
	if (!IsOnM1Cooldown)
	{
		IsOnM1Cooldown = true;
		AttackFunction();
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ADefaultCharacter::TurnAttackCooldownOff, M1Cooldown, false);
	}
}
