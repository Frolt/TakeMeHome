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
	float Damage = 0.0f;
	float CastTime = 0.0f;
	float StunDuration = 0.0f;
	EElement ElementType = EElement::E_Neutral;
	ECharacterType Owner = ECharacterType::E_Umir;
};
