// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityBase.h"
#include "BaseCharacter.h"


AAbilityBase::AAbilityBase()
{
	PrimaryActorTick.bCanEverTick = false;

}

void AAbilityBase::BeginPlay()
{
	Super::BeginPlay();

	GameInstance = AbilityOwner->GameInstance;
}
