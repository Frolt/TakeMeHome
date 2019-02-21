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

	// Set particles
	StunParticle->SetTemplate(GameInstance->StunParticle);
	CastParticle->SetTemplate(GameInstance->CastingParticle);
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

float ABaseCharacter::TakeDamage(float Damage, const FDamageEvent &DamageEvent, AController *EventInstigator, AActor *DamageCauser)
{
	// Check if dead
	if (bIsDead) return Damage;

	// Interrupt lock and casting
	if (Damage > 0.0f)
	{
		InterruptLock();
		InterruptCasting();
	}

	// Take damage/heal
	float DamageMultiplier = GetDamageMultiplier(DamageEvent.DamageTypeClass);
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage * DamageMultiplier, 0.0f, MaxHealth);

	// TODO consider adding sound/particle/animation when taking damage or healing

	// Check if character died
	if (FMath::IsNearlyZero(CurrentHealth))
	{
		bIsDead = true;
		OnDeathDelegate.Broadcast();
	}

	return DamageMultiplier;
}

float ABaseCharacter::GetDamageMultiplier(TSubclassOf<UDamageType> DamageType)
{
	if (DamageType == GameInstance->FireDamage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Took fire damage"));

		if (ElementType == EElement::E_Fire) return 0.5f;
		if (ElementType == EElement::E_Nature) return 2.0f;
	}
	else if (DamageType == GameInstance->WaterDamage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Took water damage"));

		if (ElementType == EElement::E_Water) return 0.5f;
		if (ElementType == EElement::E_Fire) return 2.0f;
	}
	else if (DamageType == GameInstance->NatureDamage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Took nature damage"));

		if (ElementType == EElement::E_Nature) return 0.5f;
		if (ElementType == EElement::E_Earth) return 2.0f;
	}
	else if (DamageType == GameInstance->EarthDamage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Took earth damage"));

		if (ElementType == EElement::E_Earth) return 0.5f;
		if (ElementType == EElement::E_Lightning) return 2.0f;
	}
	else if (DamageType == GameInstance->LightningDamage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Took lightning damage"));

		if (ElementType == EElement::E_Lightning) return 0.5f;
		if (ElementType == EElement::E_Water) return 2.0f;
	}
	return 1.0f;
}

void ABaseCharacter::OnDeath()
{

}

void ABaseCharacter::Heal(float Amount, bool bIsPassive)
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

void ABaseCharacter::RestoreMana(float Amount, bool bIsPassive)
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
	if (!ensureMsgf(!FMath::IsNearlyZero(CastDuration), TEXT("CastDuration: %f"), CastDuration)) return;

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
	if (!ensureMsgf(!FMath::IsNearlyZero(LockDuration), TEXT("LockDuration: %f"), LockDuration)) return;

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
	if (!ensureMsgf(!FMath::IsNearlyZero(StunDuration), TEXT("StunDuration: %f"), StunDuration)) return;

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

	auto *Spell = GameInstance->GetDefensiveSpell(Key);

	// Spawn defensive spell
	auto *SpawnedActor = GetWorld()->SpawnActorDeferred<ADefensiveSpellBase>(Spell->ClassRef, SpawnTransform);
	SpawnedActor->AbilityOwner = this;
	SpawnedActor->Damage = Spell->Damage;
	SpawnedActor->LockTime = Spell->LockTime;
	SpawnedActor->FinishSpawning(SpawnTransform);
}

void ABaseCharacter::UseOffensiveSpell(EOffensiveSpell SpellKey, FTransform SpawnTransform)
{
	if (!ensure(SpellKey != EOffensiveSpell::OS_None)) return;
	if (!bCanUseSpell) return;

	auto *Spell = GameInstance->GetOffensiveSpell(SpellKey);

	// Spawning offensive spell
	auto SpawnedActor = GetWorld()->SpawnActorDeferred<AOffensiveSpellBase>(Spell->ClassRef, SpawnTransform);
	SpawnedActor->AbilityOwner = this;
	SpawnedActor->Damage = Spell->Damage;
	SpawnedActor->CastTime = Spell->CastTime;
	SpawnedActor->StunDuration = Spell->StunDuration;
	SpawnedActor->DecalRadius = Spell->DecalRadius;
	Spell->ElementType == EElement::E_Neutral ? SpawnedActor->ElementType = ActiveElement : SpawnedActor->ElementType = Spell->ElementType;
	SpawnedActor->FinishSpawning(SpawnTransform);
}

void ABaseCharacter::UsePhysicalAttack(EPhysicalAttack Key)
{
	if (!ensure(Key != EPhysicalAttack::PA_None)) return;
	if (!bCanUseSpell) return;
	
	auto *PhysicalAttack = GameInstance->GetPhysicalAttack(Key);

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
