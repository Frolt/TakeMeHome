// Fill out your copyright notice in the Description page of Project Settings.

#include "PhysicalAttackBase.h"
#include "TakeMeHomeGameInstance.h"


APhysicalAttackBase::APhysicalAttackBase()
{
	PrimaryActorTick.bCanEverTick = false;

}

void APhysicalAttackBase::BeginPlay()
{
	Super::BeginPlay();

	DamageType = GameInstance->PhysicalDamage;
}
