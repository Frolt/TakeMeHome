// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy.h"


AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = false;

}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	OnDeathDelegate.AddUniqueDynamic(this, &ABaseCharacter::OnNPCDeath);
}
