// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityBase.h"
#include "TakeMeHomeEnums.h"
#include "OffensiveSpellBase.generated.h"


UCLASS()
class TAKEMEHOME_API AOffensiveSpellBase : public AAbilityBase
{
	GENERATED_BODY()

public:
	AOffensiveSpellBase();
	virtual void BeginPlay() override;

public:
	UPROPERTY(BlueprintReadOnly, Category = "Spell Properties")
	float Damage = 0.0f;
	UPROPERTY(BlueprintReadOnly, Category = "Spell Properties")
	float CastTime = 0.0f;
	UPROPERTY(BlueprintReadOnly, Category = "Spell Properties")
	float StunDuration = 0.0f;
	UPROPERTY(BlueprintReadOnly, Category = "Spell Properties")
	float DecalRadius = 0.0f;
	UPROPERTY(BlueprintReadOnly, Category = "Spell Properties")
	EElement ElementType = EElement::E_Neutral;
};
