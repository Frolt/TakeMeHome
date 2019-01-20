// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedEnum.h"
#include "TakeMeHomeEnums.generated.h"

/**
 * Contains all enums for the game
 */

UENUM(BlueprintType)
enum class EElement : uint8
{
	E_Fire 			UMETA(DisplayName = "Fire"),
	E_Nature 		UMETA(DisplayName = "Nature"),
	E_Water			UMETA(DisplayName = "Water"),
	E_Earth			UMETA(DisplayName = "Earth"),
	E_Lightning		UMETA(DisplayName = "Lightning"),
	E_None			UMETA(DisplayName = "None")
};

UENUM(BlueprintType)
enum class ESpellOwner : uint8
{
	E_Umir 			UMETA(DisplayName = "Umir"),
	E_Verdant 		UMETA(DisplayName = "Verdant"),
	E_Enemy			UMETA(DisplayName = "Enemy")
};

UCLASS()
class TAKEMEHOME_API UTakeMeHomeEnums : public UUserDefinedEnum
{
	GENERATED_BODY()
	
};
