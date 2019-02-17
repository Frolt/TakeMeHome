// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "TakeMeHomeEnums.h"
#include "Enemy.generated.h"


UCLASS()
class TAKEMEHOME_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();
	virtual void BeginPlay() override;

public:
	// Abilities
	EPhysicalAttack MeleeAttack = EPhysicalAttack::PA_Enemy_Attack;
};
