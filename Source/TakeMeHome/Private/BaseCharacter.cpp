// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseCharacter.h"
#include "TakeMeHomeGameInstance.h"


ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Find game instance
	GameInstance = Cast<UTakeMeHomeGameInstance>(GetGameInstance());	
}

float ABaseCharacter::GetHealthPercentage() const
{
	return CurrentHealth / MaxHealth;
}

float ABaseCharacter::GetManaPercentage() const
{
	return CurrentMana / MaxMana;
}

void ABaseCharacter::RestoreMovement()
{
	bStopMovement = false;
	bUseControllerRotationYaw = false;
}
