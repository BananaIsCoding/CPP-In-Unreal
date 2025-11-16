// Fill out your copyright notice in the Description page of Project Settings.


#include "StandardFgoDamageType.h"

void UStandardFgoDamageType::Init(EFgoClassType FgoClass)
{
	ClassOfAttacker = FgoClass;
}

float UStandardFgoDamageType::CalcClassAdvantage(EFgoClassType ClassOfDefender)
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

float UStandardFgoDamageType::CalcDamageTaken(float BaseDamage, EFgoClassType ClassOfDefender)
{
	return BaseDamage * CalcClassAdvantage(ClassOfDefender);
}
