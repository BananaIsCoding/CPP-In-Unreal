// Fill out your copyright notice in the Description page of Project Settings.


#include "TutorialStuff/HealthComponent.h"


// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;

	GetOwner()->OnTakeAnyDamage.AddUniqueDynamic(this, &UHealthComponent::OnDamaged);
}

// calculate the advantage percentage
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
float UHealthComponent::CalcCardAdvantage(ECardType AttackerCardType)
{
	if (AttackerCardType == ChosenCard)
	{
		return 0.0f;
	}
	switch (AttackerCardType)
	{
		case ECardType::Quick:
			switch ( ChosenCard)
			{
				case ECardType::Art:
					return -0.2f;
				break;
				case ECardType::Buster:
					return 0.2f;
				break;
				default:
				break;
			}
		break;
		case ECardType::Art:
			switch ( ChosenCard)
			{
				case ECardType::Quick:
					return 0.2f;
				break;
				case ECardType::Buster:
					return -0.2f;
				break;
				default:
				break;
			}
		break;
		case ECardType::Buster:
			switch ( ChosenCard)
			{
				case ECardType::Quick:
					return -0.2f;
				break;
				case ECardType::Art:
					return 0.2f;
				break;
				default:
				break;
				
			}
		break;
		default:
		break;
	}
	return 0;
}

// functions to call to deal damage
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

	if (OnDamageDone.IsBound())
		OnDamageDone.Broadcast();
	if (CurrentHealth <= 0.0f)
	{
		if (OnDeath.IsBound())
			OnDeath.Broadcast();
	}
}


