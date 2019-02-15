// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PhysicalAttackBase.generated.h"

UCLASS()
class TAKEMEHOME_API APhysicalAttackBase : public AActor
{
	GENERATED_BODY()
	
public:	
	APhysicalAttackBase();

public:
	UPROPERTY(BlueprintReadOnly, Category = "Spell Properties")
	float Damage = 0.0f;
	UPROPERTY(BlueprintReadOnly, Category = "Spell Properties")
	float CastTime = 0.0f;
	UPROPERTY(BlueprintReadOnly, Category = "Spell Properties")
	float StunDuration = 0.0f;
	UPROPERTY(BlueprintReadOnly, Category = "Spell Properties")
	AController *AbilityOwner = nullptr;
};
