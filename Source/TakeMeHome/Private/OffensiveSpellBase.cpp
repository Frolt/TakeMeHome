// Fill out your copyright notice in the Description page of Project Settings.

#include "OffensiveSpellBase.h"
#include "TakeMeHomeGameInstance.h"


AOffensiveSpellBase::AOffensiveSpellBase()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AOffensiveSpellBase::BeginPlay()
{
	Super::BeginPlay();

	switch (ElementType)
	{
	case EElement::E_Neutral:
		// TODO remove NEUTRAL condition
		break;
	case EElement::E_Fire:
		DamageType = GameInstance->FireDamage;
		break;
	case EElement::E_Nature:
		DamageType = GameInstance->NatureDamage;
		break;
	case EElement::E_Water:
		DamageType = GameInstance->WaterDamage;
		break;
	case EElement::E_Earth:
		DamageType = GameInstance->EarthDamage;
		break;
	case EElement::E_Lightning:
		DamageType = GameInstance->LightningDamage;
		break;
	}
}
