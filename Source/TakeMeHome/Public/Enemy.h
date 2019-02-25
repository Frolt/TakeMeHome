// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "TakeMeHomeEnums.h"
#include "Enemy.generated.h"

class UWidgetComponent;


UCLASS()
class TAKEMEHOME_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();
	virtual void BeginPlay() override;
	virtual float TakeDamage(float Damage, const FDamageEvent &DamageEvent, AController *EventInstigator, AActor *DamageCauser) override;
	virtual void OnDeath() override;
	void SetMaterialAccordingToElement();
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat Text")
	void SpawnCombatText(float Damage, float FontScale, FLinearColor Color);
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat Text")
	void DeathEvent();

public:
	// Abilities
	EPhysicalAttack MeleeAttack = EPhysicalAttack::PA_Enemy_Attack;
	UPROPERTY(BlueprintReadOnly, Category = "Aggro")
	bool bWasAttacked = false;
};
