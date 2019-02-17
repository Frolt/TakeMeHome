// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityBase.h"
#include "DefensiveSpellBase.generated.h"

/**
 * 
 */
UCLASS()
class TAKEMEHOME_API ADefensiveSpellBase : public AAbilityBase
{
	GENERATED_BODY()
	
public:
	ADefensiveSpellBase();

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings")
	float Damage = 0.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Settings")
	float LockTime = 0.0f;
};
