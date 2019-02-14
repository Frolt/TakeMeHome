// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseCharacter.h"
#include "TakeMeHomeGameInstance.h"
#include "Engine/World.h"
#include "Public/TimerManager.h"


ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Find game instance
	GameInstance = Cast<UTakeMeHomeGameInstance>(GetGameInstance());	
}

void ABaseCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Passive regen TODO check if out of combat
	if (!bIsDead)
	{
		CurrentMana = FMath::Min(CurrentMana + PassiveManaRegenPerSecond * DeltaSeconds, MaxMana);
		CurrentHealth = FMath::Min(CurrentHealth + PassiveHealthRegenPerSecond * DeltaSeconds, MaxHealth);
	}
}

float ABaseCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// Interrupt casting
	InterruptCasting();

	// Take damage
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, MaxHealth);

	// Check if died
	if (FMath::IsNearlyEqual(CurrentHealth, 0.0f))
	{
		bIsDead = true;
		OnDeathDelegate.Broadcast();
	}

	return Damage;
}

void ABaseCharacter::OnNPCDeath()
{
	UE_LOG(LogTemp, Warning, TEXT("%s died (NPC)"), *GetName());
	DetachFromControllerPendingDestroy();
}

float ABaseCharacter::GetHealthPercentage() const
{
	return CurrentHealth / MaxHealth;
}

float ABaseCharacter::GetManaPercentage() const
{
	return CurrentMana / MaxMana;
}

void ABaseCharacter::StartCasting(float CastDuration)
{
	TimeCastingBegan = GetWorld()->GetTimeSeconds();
	TimeCastingEnds = TimeCastingBegan + CastDuration;
	bIsCasting = true;
	bCanUseSpell = false;
	GetWorldTimerManager().SetTimer(CastTimerHandle, this, &ABaseCharacter::CastSuccesfull, CastDuration);
}

bool ABaseCharacter::InterruptCasting()
{
	if (bIsCasting)
	{
		bIsCasting = false;
		GetWorldTimerManager().ClearTimer(CastTimerHandle);
		bCanUseSpell = true;
		OnCastingStatusChange.Broadcast(false);
		return true;
	}
	else
	{
		return false;
	}
}

void ABaseCharacter::CastSuccesfull()
{
	GetWorldTimerManager().ClearTimer(CastTimerHandle);
	bCanUseSpell = true;
	bIsCasting = false;
	OnCastingStatusChange.Broadcast(true);
}

void ABaseCharacter::LockCharacter()
{
	bCanMove = false;
	bCanUseSpell = false;
	bUseControllerRotationYaw = true;
}

void ABaseCharacter::RestoreCharacter()
{
	bCanMove = true;
	bCanUseSpell = true;
	bUseControllerRotationYaw = false;
	
}
