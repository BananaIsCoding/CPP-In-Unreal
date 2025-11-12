#pragma once

#include "CoreMinimal.h"
#include "CardPoolItem.generated.h"

USTRUCT(BlueprintType)
struct FCardPoolItem
{
	GENERATED_BODY()

	int CharIndex;

	int SkillIndex;
};
