// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseCharacter.h"
#include "Engine/World.h"
#include "Public/TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "TakeMeHomeGameInstance.h"
#include "PotionBase.h"
#include "DefensiveSpellBase.h"
#include "PhysicalAttackBase.h"
#include "OffensiveSpellBase.h"


ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create particle systems
	StunParticle = CreateDefaultSubobject<UParticleSystemComponent>(FName("Stun Particle"));
	StunParticle->SetupAttachment(RootComponent);
	StunParticle->SetVisibility(false);
	CastParticle = CreateDefaultSubobject<UParticleSystemComponent>(FName("Cast Particle"));
	CastParticle->bAutoActivate = false;
	CastParticle->SetVisibility(false);
	CastParticle->SetupAttachment(RootComponent);

}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Find game instance
	GameInstance = Cast<UTakeMeHomeGameInstance>(GetGameInstance());	

	// Set particles
	StunParticle->SetTemplate(GameInstance->StunParticle);
	CastParticle->SetTemplate(GameInstance->CastingParticle);
}

void ABaseCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	PassiveRegen(DeltaSeconds);
}

void ABaseCharacter::PassiveRegen(float DeltaSeconds)
{
	if (!bIsDead)
	{
		Heal(PassiveHealthRegenPerSecond * DeltaSeconds);
		RestoreMana(PassiveManaRegenPerSecond * DeltaSeconds);
	}
}

float ABaseCharacter::TakeDamage(float Damage, const FDamageEvent &DamageEvent, AController *EventInstigator, AActor *DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("hei"));
	// Check if dead
	if (bIsDead) return Damage;
	if (!bCanBeDamaged) return 0.0f;

	// Interrupt lock and casting
	if (Damage > 0.0f)
	{
		InterruptLock();
		InterruptCasting();
		OnPhysicalDamageRecieved.Broadcast();
	}

	// Take damage/heal
	float DamageMultiplier = GetDamageMultiplier(DamageEvent.DamageTypeClass);
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage * DamageMultiplier, 0.0f, MaxHealth);

	// Play hit animation
	if (DamageEvent.DamageTypeClass == GameInstance->PhysicalDamage)
	{
		//OnPhysicalAttack.Broadcast()
		// Fire hit event to animation BP
	}

	// TODO consider adding sound/particle/animation when taking damage or healing

	// Check if character died
	if (FMath::IsNearlyZero(CurrentHealth))
	{
		bIsDead = true;
		OnDeathDelegate.Broadcast();
	}

	return Damage * DamageMultiplier;
}

float ABaseCharacter::GetDamageMultiplier(TSubclassOf<UDamageType> DamageType)
{
	if (DamageType == GameInstance->FireDamage)
	{
		if (ElementType == EElement::E_Fire) return 0.5f;
		if (ElementType == EElement::E_Nature) return 2.0f;
	}
	else if (DamageType == GameInstance->WaterDamage)
	{
		if (ElementType == EElement::E_Water) return 0.5f;
		if (ElementType == EElement::E_Fire) return 2.0f;
	}
	else if (DamageType == GameInstance->NatureDamage)
	{
		if (ElementType == EElement::E_Nature) return 0.5f;
		if (ElementType == EElement::E_Water) return 2.0f;
	}
	return 1.0f;
}

void ABaseCharacter::OnDeath()
{

}

void ABaseCharacter::Heal(float Amount, bool bIsPassive /*= true*/)
{
	CurrentHealth = FMath::Clamp(CurrentHealth + Amount, 0.0f, MaxHealth);

	if (!bIsPassive)
	{
		SpawnHealthManaText(Amount, 1.0f, GameInstance->HealColor);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), GameInstance->HealingParticle, CastParticle->GetComponentTransform());
	}
}

void ABaseCharacter::DrainMana(float Amount)
{
	CurrentMana = FMath::Clamp(CurrentMana - Amount, 0.0f, MaxMana);
}

void ABaseCharacter::RestoreMana(float Amount, bool bIsPassive /*= true*/)
{
	CurrentMana = FMath::Clamp(CurrentMana + Amount, 0.0f, MaxMana);

	if (!bIsPassive)
	{
		SpawnHealthManaText(Amount, 1.0f, GameInstance->ManaColor);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), GameInstance->ManaParticle, CastParticle->GetComponentTransform());
	}
}

float ABaseCharacter::GetHealthPercentage() const
{
	return CurrentHealth / MaxHealth;
}

float ABaseCharacter::GetManaPercentage() const
{
	return CurrentMana / MaxMana;
}

bool ABaseCharacter::DrainStamina(int32 Amount)
{
	if (CurrentStamina >= Amount)
	{
		CurrentStamina -= Amount;
		GetWorldTimerManager().SetTimer(StaminaRegenTimer, this, &ABaseCharacter::RestoreStamina, StaminaRegenInterval, true);
		return true;
	}
	else
	{
		return false;
	}
}

void ABaseCharacter::RestoreStamina()
{
	CurrentStamina++;
	if (CurrentStamina == MaxStamina)
	{
		GetWorldTimerManager().ClearTimer(StaminaRegenTimer);
	}
}

float ABaseCharacter::GetStaminaPercentage() const
{
	return static_cast<float>(CurrentStamina) / static_cast<float>(MaxStamina);
}

float ABaseCharacter::GetHealth() const
{
	return CurrentHealth;
}

float ABaseCharacter::GetMana() const
{
	return CurrentMana;
}

void ABaseCharacter::StartCasting(float CastDuration, bool bCanInterrupt /*= true*/)
{
	if (FMath::IsNearlyZero(CastDuration)) return;

	bCanInterruptCasting = bCanInterrupt;
	TimeCastingBegan = GetWorld()->GetTimeSeconds();
	TimeCastingEnds = TimeCastingBegan + CastDuration;
	bIsCasting = true;
	bCanUseSpell = false;
	CastTime = CastDuration;
	GetWorldTimerManager().SetTimer(CastTimer, this, &ABaseCharacter::CastSuccesfull, CastDuration);

	// Add stun particle
	if (!ensure(CastParticle)) return;
	CastParticle->ActivateSystem();
	CastParticle->SetVisibility(true);
}

bool ABaseCharacter::InterruptCasting()
{
	if (!bCanInterruptCasting)
	{
		return false;
	}
	if (bIsCasting)
	{
		bIsCasting = false;
		bCanUseSpell = true;
		OnCastingStatusChange.Broadcast(false);
		GetWorldTimerManager().ClearTimer(CastTimer);

		// Remove stun particle
		if (!ensure(CastParticle)) return false;
		CastParticle->DeactivateSystem();
		CastParticle->SetVisibility(false);

		return true;
	}
	else
	{
		return false;
	}
}

void ABaseCharacter::CastSuccesfull()
{
	bCanInterruptCasting = true;
	bIsCasting = false;
	bCanUseSpell = true;
	OnCastingStatusChange.Broadcast(true);
	GetWorldTimerManager().ClearTimer(CastTimer);

	// Remove cast particle
	if (!ensure(CastParticle)) return;
	CastParticle->DeactivateSystem();
	CastParticle->SetVisibility(false);
}

void ABaseCharacter::LockCharacter(float LockDuration, bool bDisableMovement /*= true*/)
{
	if (FMath::IsNearlyZero(LockDuration)) return;

	if (bDisableMovement)
	{
		bCanMove = false;
	}
	bCanUseSpell = false;
	bIsLocked = true;
	GetWorldTimerManager().SetTimer(LockTimer, this, &ABaseCharacter::UnlockCharacter, LockDuration);
}

void ABaseCharacter::UnlockCharacter()
{
	if (bIsStunned) return;

	bCanMove = true;
	bCanUseSpell = true;
	bIsLocked = false;
	GetWorldTimerManager().ClearTimer(LockTimer);
}

void ABaseCharacter::InterruptLock()
{
	UnlockCharacter();
	if (bIsLocked)
	{
		OnLockInterrupted.Broadcast();
	}
}

void ABaseCharacter::Stun(float StunDuration)
{
	if (!ensure(!FMath::IsNearlyZero(StunDuration))) return;

	// Override previous stun
	GetWorldTimerManager().ClearTimer(StunTimer);

	bIsStunned = true;
	bCanMove = false;
	bCanUseSpell = false;
	GetWorldTimerManager().SetTimer(StunTimer, this, &ABaseCharacter::InterruptStun, StunDuration);
	InterruptCasting();
	InterruptLock();

	// Fire stun event
	OnStunned.Broadcast();

	// Add stun particle
	if (!ensure(StunParticle)) return;
	StunParticle->SetVisibility(true);

}

void ABaseCharacter::InterruptStun()
{
	if (!bIsStunned) return;

	bIsStunned = false;
	bCanMove = true;
	bCanUseSpell = true;
	GetWorldTimerManager().ClearTimer(StunTimer);

	// Remove stun particle
	if (!ensure(StunParticle)) return;
	StunParticle->SetVisibility(false);
}

void ABaseCharacter::UsePotion(EPotion Key)
{
	if (!ensure(Key != EPotion::P_None)) return;
	if (!bCanUseSpell) return;

	// Get potion
	auto *Potion = GameInstance->GetPotion(Key);

	// Spawn potion
	auto SpawnedActor = GetWorld()->SpawnActorDeferred<APotionBase>(Potion->ClassRef, GetActorTransform());
	SpawnedActor->AbilityOwner = this;
	SpawnedActor->LockTime = Potion->LockTime;
	SpawnedActor->FinishSpawning(GetActorTransform());
}

void ABaseCharacter::UseDefensiveSpell(EDefensiveSpell Key, FTransform SpawnTransform)
{
	if (!ensure(Key != EDefensiveSpell::DS_None)) return;
	if (!bCanUseSpell) return;

	// Get spell
	auto *Spell = GameInstance->GetDefensiveSpell(Key);

	// Spawn spell
	auto *SpawnedActor = GetWorld()->SpawnActorDeferred<ADefensiveSpellBase>(Spell->ClassRef, SpawnTransform);
	SpawnedActor->AbilityOwner = this;
	SpawnedActor->Damage = Spell->Damage;
	SpawnedActor->LockTime = Spell->LockTime;
	SpawnedActor->FinishSpawning(SpawnTransform);
}

AActor * ABaseCharacter::UseOffensiveSpell(EOffensiveSpell SpellKey, FTransform SpawnTransform)
{
	if (!ensure(SpellKey != EOffensiveSpell::OS_None)) return nullptr;
	if (!bCanUseSpell) return nullptr;

	// Get spell
	auto *Spell = GameInstance->GetOffensiveSpell(SpellKey);

	// Spawning spell
	auto SpawnedActor = GetWorld()->SpawnActorDeferred<AOffensiveSpellBase>(Spell->ClassRef, SpawnTransform);
	SpawnedActor->AbilityOwner = this;
	SpawnedActor->Damage = Spell->Damage;
	SpawnedActor->CastTime = Spell->CastTime;
	SpawnedActor->StunDuration = Spell->StunDuration;
	SpawnedActor->DecalRadius = Spell->DecalRadius;
	Spell->ElementType == EElement::E_Neutral ? SpawnedActor->ElementType = ActiveElement : SpawnedActor->ElementType = Spell->ElementType;
	SpawnedActor->FinishSpawning(SpawnTransform);
	return SpawnedActor;
}

bool ABaseCharacter::UsePhysicalAttack(EPhysicalAttack Key)
{
	if (!ensure(Key != EPhysicalAttack::PA_None)) return false;
	if (!bCanUseSpell) return false;
	
	// Get attack
	auto *PhysicalAttack = GameInstance->GetPhysicalAttack(Key);

	// Check for stamina
	if (CurrentStamina >= PhysicalAttack->StaminaCost)
	{
		DrainStamina(PhysicalAttack->StaminaCost);
	}
	else
	{
		return false;
	}

	// Spawn attack
	auto *SpawnedActor = GetWorld()->SpawnActorDeferred<APhysicalAttackBase>(PhysicalAttack->ClassRef, GetActorTransform());
	SpawnedActor->AbilityOwner = this;
	SpawnedActor->Damage = PhysicalAttack->Damage;
	SpawnedActor->LockTime = PhysicalAttack->LockTime;
	SpawnedActor->Delay = PhysicalAttack->Delay;
	SpawnedActor->StunDuration = PhysicalAttack->StunDuration;
	SpawnedActor->bDisableMovementWhileAttacking = PhysicalAttack->bDisableMovementWhileAttacking;
	SpawnedActor->FinishSpawning(GetActorTransform());

	// Fire animation event
	switch (Key)
	{
	case EPhysicalAttack::PA_Fast_Attack:
		OnPhysicalAttackStarted.Broadcast(true);
		break;
	case EPhysicalAttack::PA_Slow_Attack:
		OnPhysicalAttackStarted.Broadcast(false);
		break;
	case EPhysicalAttack::PA_Enemy_Attack:
		OnPhysicalAttackStarted.Broadcast(false);
		break;	
	case EPhysicalAttack::PA_HW_Attack:
		OnPhysicalAttackStarted.Broadcast(false);
		break;
	}

	return true;
}

void ABaseCharacter::BroadcastCounterStrike()
{
	OnCounterStrikeActivated.Broadcast();
}

void ABaseCharacter::ChangeActiveElement(EElement Element, float Duration)
{
	ActiveElement = Element;

	GetWorldTimerManager().SetTimer(ResetActiveElementTimer, this, &ABaseCharacter::ResetActiveElement, Duration);
	TimeActiveElementExpires = GetWorld()->GetTimeSeconds() + Duration;
}

void ABaseCharacter::ResetActiveElement()
{
	UE_LOG(LogTemp, Warning, TEXT("resetting active element!"));
	ActiveElement = EElement::E_Neutral;
}
