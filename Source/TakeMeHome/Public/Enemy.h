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
	virtual float TakeDamage(float Damage, const FDamageEvent &DamageEvent, AController *EventInstigator, AActor *DamageCauser) override;
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat Text")
	void SpawnCombatText(float Damage, float FontScale, FLinearColor Color);

	void SetMaterialAccordingToElement();

public:
	// Abilities
	EPhysicalAttack MeleeAttack = EPhysicalAttack::PA_Enemy_Attack;
};
