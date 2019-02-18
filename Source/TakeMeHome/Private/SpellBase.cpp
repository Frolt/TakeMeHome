// Fill out your copyright notice in the Description page of Project Settings.

#include "SpellBase.h"
#include "TakeMeHomeGameInstance.h"


ASpellBase::ASpellBase()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ASpellBase::BeginPlay()
{
	Super::BeginPlay();

	switch (ElementType)
	{
	case EElement::E_Neutral:
		// TODO remove NEUTRAL condition
		break;
	case EElement::E_Fire:
		DamageEvent.DamageTypeClass = GameInstance->FireDamage;
		break;
	case EElement::E_Nature:
		DamageEvent.DamageTypeClass = GameInstance->NatureDamage;
		break;
	case EElement::E_Water:
		DamageEvent.DamageTypeClass = GameInstance->WaterDamage;
		break;
	case EElement::E_Earth:
		DamageEvent.DamageTypeClass = GameInstance->EarthDamage;
		break;
	case EElement::E_Lightning:
		DamageEvent.DamageTypeClass = GameInstance->LightningDamage;
		break;
	}
}
