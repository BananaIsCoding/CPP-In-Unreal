#pragma once

#include "CoreMinimal.h"
#include "FgoCardTypeEnum.generated.h"

UENUM(BlueprintType)
enum class ECardType : uint8 
{
	None UMETA(Hidden),
	Quick UMETA(DisplayName = "Quick"),
	Art UMETA(DisplayName = "Art"),
	Buster UMETA(DisplayName = "Buster")
};
