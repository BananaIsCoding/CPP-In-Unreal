#pragma once

#include "CoreMinimal.h"
#include "FgoClassTypeEnum.generated.h"

UENUM(BlueprintType)
enum class EFgoClassType : uint8 
{
	None UMETA(Hidden),
	Saber UMETA(DisplayName = "Saber"),
	Lancer UMETA(DisplayName = "Lancer"),
	Archer UMETA(DisplayName = "Archer"),
	Rider UMETA(DisplayName = "Rider"),
	Caster UMETA(DisplayName = "Caster"),
	Assassin UMETA(DisplayName = "Assassin"),
	Berserker UMETA(DisplayName = "Berserker")
};
