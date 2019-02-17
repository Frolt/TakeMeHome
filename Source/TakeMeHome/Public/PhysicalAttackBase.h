// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityBase.h"
#include "PhysicalAttackBase.generated.h"

UCLASS()
class TAKEMEHOME_API APhysicalAttackBase : public AAbilityBase
{
	GENERATED_BODY()
	
public:	
	APhysicalAttackBase();

public:
	UPROPERTY(BlueprintReadWrite, Category = "Spell Properties")
	float Damage = 0.0f;
	UPROPERTY(BlueprintReadOnly, Category = "Spell Properties")
	float LockTime = 0.0f;
	UPROPERTY(BlueprintReadOnly, Category = "Spell Properties")
	float Delay = 0.0f;
	UPROPERTY(BlueprintReadOnly, Category = "Spell Properties")
	float StunDuration = 0.0f;
};
