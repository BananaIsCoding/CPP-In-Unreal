#pragma once

#include "CoreMinimal.h"
#include "Why_FGO_Aint_3D/MyGameStuff/Enums/FgoClassTypeEnum.h"
#include "InfoStruct.generated.h"

USTRUCT(BlueprintType)
struct FInfoStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	FName Name;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	float Health = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	float Damage = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	EFgoClassType Class = EFgoClassType::None;
};
