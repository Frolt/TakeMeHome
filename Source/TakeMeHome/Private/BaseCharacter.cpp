// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseCharacter.h"
#include "TakeMeHomeGameInstance.h"
#include "Engine/World.h"
#include "Public/TimerManager.h"
#include "PotionBase.h"
#include "DefensiveSpellBase.h"
#include "PhysicalAttackBase.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "SpellBase.h"


ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create particle systems
	StunParticle = CreateDefaultSubobject<UParticleSystemComponent>(FName("Stun Particle"));
	StunParticle->SetupAttachment(RootComponent);
	StunParticle->bAutoActivate = false;
	CastParticle = CreateDefaultSubobject<UParticleSystemComponent>(FName("Cast Particle"));
	CastParticle->bAutoActivate = false;
	CastParticle->SetupAttachment(RootComponent);

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
		Heal(PassiveHealthRegenPerSecond * DeltaSeconds);
		RestoreMana(PassiveManaRegenPerSecond * DeltaSeconds);
	}
}

float ABaseCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// Interrupt lock and casting
	if (Damage > 0.0f)
	{
		InterruptLock();
		InterruptCasting();
	}

	// TODO consider adding sound/particle/animation when taking damage or healing

	// Take damage/heal
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
	GetMesh()->SetSimulatePhysics(true);
	DetachFromControllerPendingDestroy();
}

void ABaseCharacter::Heal(float Amount)
{
	CurrentHealth = FMath::Clamp(CurrentHealth + Amount, 0.0f, MaxHealth);
}

void ABaseCharacter::DrainMana(float Amount)
{
	CurrentMana = FMath::Clamp(CurrentMana - Amount, 0.0f, MaxMana);
}

void ABaseCharacter::RestoreMana(float Amount)
{
	CurrentMana = FMath::Clamp(CurrentMana + Amount, 0.0f, MaxMana);
}

float ABaseCharacter::GetHealthPercentage() const
{
	return CurrentHealth / MaxHealth;
}

float ABaseCharacter::GetManaPercentage() const
{
	return CurrentMana / MaxMana;
}

float ABaseCharacter::GetHealth() const
{
	return CurrentHealth;
}

float ABaseCharacter::GetMana() const
{
	return CurrentMana;
}

void ABaseCharacter::StartCasting(float CastDuration)
{
	if (ensure(FMath::IsNearlyZero(CastDuration))) return;

	TimeCastingBegan = GetWorld()->GetTimeSeconds();
	TimeCastingEnds = TimeCastingBegan + CastDuration;
	bIsCasting = true;
	bCanUseSpell = false;
	GetWorldTimerManager().SetTimer(CastTimerHandle, this, &ABaseCharacter::CastSuccesfull, CastDuration);

	// Add stun particle
	if (!ensure(CastParticle)) return;
	CastParticle->ActivateSystem();
	CastParticle->SetVisibility(true);
}

bool ABaseCharacter::InterruptCasting()
{
	if (bIsCasting)
	{
		bIsCasting = false;
		bCanUseSpell = true;
		OnCastingStatusChange.Broadcast(false);
		GetWorldTimerManager().ClearTimer(CastTimerHandle);

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
	bIsCasting = false;
	bCanUseSpell = true;
	OnCastingStatusChange.Broadcast(true);
	GetWorldTimerManager().ClearTimer(CastTimerHandle);

	// Remove cast particle
	if (!ensure(CastParticle)) return;
	CastParticle->DeactivateSystem();
	CastParticle->SetVisibility(false);
}

void ABaseCharacter::LockCharacter(float LockDuration)
{
	if (ensureMsgf(FMath::IsNearlyZero(LockDuration), TEXT("LockDuration: %f"), LockDuration)) return;

	bCanMove = false;
	bCanUseSpell = false;
	bIsLocked = true;
	bUseControllerRotationYaw = false;
	GetWorldTimerManager().SetTimer(LockTimerHandle, this, &ABaseCharacter::UnlockCharacter, LockDuration);
}

void ABaseCharacter::UnlockCharacter()
{
	bCanMove = true;
	bCanUseSpell = true;
	bIsLocked = false;
	bUseControllerRotationYaw = false;
	GetWorldTimerManager().ClearTimer(LockTimerHandle);
}

void ABaseCharacter::InterruptLock()
{
	UnlockCharacter();
	OnLockInterrupted.Broadcast();
}

void ABaseCharacter::Stun(float StunDuration, bool OverrideStun)
{
	if (ensure(FMath::IsNearlyZero(StunDuration))) return;

	// Override previous stun
	if (bIsStunned)
	{
		GetWorldTimerManager().ClearTimer(StunTimerHandle);
	}

	bIsStunned = true;
	bCanMove = false;
	bCanUseSpell = false;
	GetWorldTimerManager().SetTimer(StunTimerHandle, this, &ABaseCharacter::InterruptStun, StunDuration);

	// Add stun particle
	if (!ensure(StunParticle)) return;
	StunParticle->ActivateSystem();
	StunParticle->SetVisibility(true);
}

void ABaseCharacter::InterruptStun()
{
	if (!bIsStunned) return;

	bIsStunned = false;
	bCanMove = true;
	bCanUseSpell = true;
	GetWorldTimerManager().ClearTimer(StunTimerHandle);

	// Remove stun particle
	if (!ensure(StunParticle)) return;
	StunParticle->DeactivateSystem();
	StunParticle->SetVisibility(false);
}

void ABaseCharacter::UsePotion(EPotion Key)
{
	if (!ensure(Key != EPotion::P_None)) return;
	if (!bCanUseSpell) return;

	auto *Potion = GameInstance->Potions.Find(Key);
	if (!ensure(Potion)) return;

	// Spawn potion
	auto SpawnedActor = GetWorld()->SpawnActorDeferred<APotionBase>(Potion->ClassRef, GetActorTransform());
	SpawnedActor->AbilityOwner = this;
	SpawnedActor->FinishSpawning(GetActorTransform());
}

void ABaseCharacter::UseDefensiveSpell(EDefensiveSpell Key, FTransform SpawnTransform)
{
	if (!ensure(Key != EDefensiveSpell::DS_None)) return;
	if (!bCanUseSpell) return;

	auto *Spell = GameInstance->DefensiveSpells.Find(Key);
	if (!ensure(Spell)) return;

	// Spawn defensive spell
	auto *SpawnedActor = GetWorld()->SpawnActorDeferred<ADefensiveSpellBase>(Spell->ClassRef, SpawnTransform);
	SpawnedActor->AbilityOwner = this;
	SpawnedActor->Damage = Spell->Damage;
	SpawnedActor->LockTime = Spell->LockTime;
	SpawnedActor->FinishSpawning(SpawnTransform);
}

void ABaseCharacter::UseOffensiveSpell(EOffensiveSpell Key, FTransform SpawnTransform)
{
	if (!ensure(Key != EOffensiveSpell::OS_None)) return;
	if (!bCanUseSpell) return;

	auto *Spell = GameInstance->OffensiveSpells.Find(Key);
	if (!ensure(Spell)) return;

	// Spawn offensive spell
	auto *SpawnedActor = GetWorld()->SpawnActorDeferred<ASpellBase>(Spell->ClassRef, SpawnTransform);
	SpawnedActor->AbilityOwner = this;
	SpawnedActor->Damage = Spell->Damage;
	SpawnedActor->CastTime = Spell->CastTime;
	SpawnedActor->StunDuration = Spell->StunDuration;
	SpawnedActor->DecalRadius = Spell->DecalRadius;
	SpawnedActor->ElementType == EElement::E_Neutral ? SpawnedActor->ElementType = ActiveElement : SpawnedActor->ElementType = Spell->ElementType;
	SpawnedActor->FinishSpawning(SpawnTransform);
}

void ABaseCharacter::UsePhysicalAttack(EPhysicalAttack Key)
{
	if (!ensure(Key != EPhysicalAttack::PA_None)) return;
	if (!bCanUseSpell) return;
	
	auto *PhysicalAttack = GameInstance->PhysicalAttacks.Find(Key);
	if (!ensure(PhysicalAttack)) return;

	// Spawn physical attack
	auto *SpawnedActor = GetWorld()->SpawnActorDeferred<APhysicalAttackBase>(PhysicalAttack->ClassRef, GetActorTransform());
	SpawnedActor->AbilityOwner = this;
	SpawnedActor->Damage = PhysicalAttack->Damage;
	SpawnedActor->LockTime = PhysicalAttack->LockTime;
	SpawnedActor->Delay = PhysicalAttack->Delay;
	SpawnedActor->StunDuration = PhysicalAttack->StunDuration;
	SpawnedActor->FinishSpawning(GetActorTransform());
}

void ABaseCharacter::BroadcastCounterStrike()
{
	OnCounterStrikeActivated.Broadcast();
}