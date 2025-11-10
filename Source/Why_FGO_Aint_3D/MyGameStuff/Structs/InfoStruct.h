#pragma once

#include "CoreMinimal.h"
#include "InfoStruct.generated.h"

USTRUCT(BlueprintType)
struct FInfoStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	FName Name;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
	FName Class;
	
};
