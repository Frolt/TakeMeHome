// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityBase.h"
#include "TakeMeHomeEnums.h"
#include "SpellBase.generated.h"


UCLASS()
class TAKEMEHOME_API ASpellBase : public AAbilityBase
{
	GENERATED_BODY()

public:
	ASpellBase();

public:
	UPROPERTY(BlueprintReadOnly, Category = "Spell Properties")
	float Damage = 0.0f;
	UPROPERTY(BlueprintReadOnly, Category = "Spell Properties")
	float CastTime = 0.0f;
	UPROPERTY(BlueprintReadOnly, Category = "Spell Properties")
	float StunDuration = 0.0f;
	UPROPERTY(BlueprintReadOnly, Category = "Spell Properties")
	EElement ElementType = EElement::E_Neutral;
};
