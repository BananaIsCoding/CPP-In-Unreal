// Fill out your copyright notice in the Description page of Project Settings.


#include "TutorialStuff/HealthComponent.h"


// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;

	GetOwner()->OnTakeAnyDamage.AddUniqueDynamic(this, &UHealthComponent::OnDamaged);
}

float UHealthComponent::CalcClassAdvantage(EFgoClassType ClassOfAttacker)
{
	if (ClassOfAttacker == ClassOfDefender)
	{
		return 0;
	}

	if (ClassOfAttacker == EFgoClassType::Berserker && ClassOfDefender == EFgoClassType::Berserker)
	{
		return 0.5f;
	}
	
	switch (ClassOfAttacker)
	{
		case EFgoClassType::Saber:
			switch (ClassOfDefender)
			{
				case EFgoClassType::Archer:
					return -0.5f;
				break;
				case EFgoClassType::Lancer:
					return 1.0f;
				break;
				default:
					return 0.0f;
				break;
			}
		break;
		case EFgoClassType::Archer:
			switch (ClassOfDefender)
			{
				case EFgoClassType::Lancer:
					return -0.5f;
				break;
				case EFgoClassType::Saber:
					return 1.0f;
				break;
				default:
					return 0.0f;
				break;
			}
		break;
		case EFgoClassType::Lancer:
			switch (ClassOfDefender)
			{
				case EFgoClassType::Saber:
					return -0.5f;
				break;
				case EFgoClassType::Archer:
					return 1.0f;
				break;
				default:
					return 0.0f;
				break;
			}
		break;
		case EFgoClassType::Rider:
			switch (ClassOfDefender)
			{
				case EFgoClassType::Assassin:
						return -0.5f;
				break;
				case EFgoClassType::Caster:
					return 1.0f;
				break;
				default:
						return 0.0f;
				break;
			}
		break;
		case EFgoClassType::Caster:
			switch (ClassOfDefender)
			{
				case EFgoClassType::Rider:
					return -0.5f;
				break;
				case EFgoClassType::Assassin:
					return 1.0f;
				break;
				default:
					return 0.0f;
				break;
			}
		break;
		case EFgoClassType::Assassin:
			switch (ClassOfDefender)
			{
				case EFgoClassType::Caster:
					return -0.5f;
				break;
				case EFgoClassType::Rider:
					return 1.0f;
				break;
				default:
					return 0.0f;
				break;
			}
		break;
		case EFgoClassType::Berserker:
			return 0.5f;
		break;
		default:
			return 0;
		break;
	}
}

float UHealthComponent::CalcCardAdvantage(ECardType ClassOfAttacker)
{
	return 1.0f;
}

void UHealthComponent::TakeDamage(float Damage)
{
	OnDamaged(Damage);
}

void UHealthComponent::TakeDamage(float Damage, EFgoClassType ClassOfAttacker)
{
	OnDamaged(Damage * (1 + CalcClassAdvantage(ClassOfAttacker)));
}

void UHealthComponent::TakeDamage(float Damage, EFgoClassType ClassOfAttacker, ECardType AttackCardType)
{
	OnDamaged(Damage * (1 + (CalcClassAdvantage(ClassOfAttacker) * CalcCardAdvantage(AttackCardType))));
}

void UHealthComponent::OnDamaged(AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
                                 class AController* InstigatedBy, AActor* DamageCauser)
{
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, MaxHealth);
	UE_LOG(LogTemp, Warning, TEXT("Health Current Health: %f"), CurrentHealth);
	
	if (CurrentHealth <= 0.0f)
	{
		if (OnDeath.IsBound())
			OnDeath.Broadcast();
	}
	else
	{
		if (OnDamageDone.IsBound())
			OnDamageDone.Broadcast();
	}
}

void UHealthComponent::OnDamaged(float Damage)
{
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, MaxHealth);
	UE_LOG(LogTemp, Warning, TEXT("Health Current Health: %f"), CurrentHealth);

	if (OnDamageDone.IsBound())
		OnDamageDone.Broadcast();
	if (CurrentHealth <= 0.0f)
	{
		if (OnDeath.IsBound())
			OnDeath.Broadcast();
	}
}


