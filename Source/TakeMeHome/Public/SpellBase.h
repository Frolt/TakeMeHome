// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TakeMeHomeEnums.h"
#include "SpellBase.generated.h"


UCLASS()
class TAKEMEHOME_API ASpellBase : public AActor
{
	GENERATED_BODY()

protected:
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
	UPROPERTY(BlueprintReadOnly, Category = "Spell Properties")
	AController *SpellOwner = nullptr;
};
