// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputMappingContext.h"
#include "Public/TutorialStuff/Tutorial_PlayerInterface.h"
#include "TutorialStuff/HealthComponent.h"
#include "DefaultCharacter.generated.h"

class USphereComponent;
class UFloatingPawnMovement;

UCLASS()
class WHY_FGO_AINT_3D_API ADefaultCharacter : public ACharacter, public ITutorial_PlayerInterface
{
	GENERATED_BODY()
	
	// makes everything under this private by default
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess="true"))
	class UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess="true"))
	class USpringArmComponent* SpringArm;

	
	 
public:
	// Sets default values for this character's properties
	ADefaultCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UHealthComponent> HealthComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void SetActorOverlap_Implementation(AActor* OverlappedActor) override;
	
protected:

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Variables From C++");
	TObjectPtr<AActor> OverlappedActorRef;
	
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputMappingContext* InputMapping;
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, Category = "EnhancedInput")
	class UInputAction* LookAction;

	void MoveFunction(const FInputActionValue& InputValue);
	void LookFunction(const FInputActionValue& InputValue);
	void JumpFunction();
};
