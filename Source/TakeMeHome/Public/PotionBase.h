// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityBase.h"
#include "PotionBase.generated.h"

class ABaseCharacter;

UCLASS()
class TAKEMEHOME_API APotionBase : public AAbilityBase
{
	GENERATED_BODY()
	
public:	
	APotionBase();
	UPROPERTY(BlueprintReadOnly, Category = "Setup")
	float LockTime = 0.0f;
};
