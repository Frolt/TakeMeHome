// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TakeMeHomeEnums.h"
#include "BaseCharacter.generated.h"

class UTakeMeHomeGameInstance;
class UParticleSystem;
class UParticleSystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInterruptLockDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCounterStrikeActivate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCastingFinishedDelegate, bool, bWasSuccessful);

UCLASS()
class TAKEMEHOME_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseCharacter();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	// Damage
	virtual float TakeDamage(float Damage, const FDamageEvent &DamageEvent, AController *EventInstigator, AActor *DamageCauser) override;
	float GetDamageMultiplier(const FDamageEvent &DamageEvent);

	// Death event
	UFUNCTION()
	void OnNPCDeath();

	// Health/Mana
	UFUNCTION(BlueprintCallable, Category = "Health")
	void Heal(float Amount, bool bIsPassive = true);
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealth() const;
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealthPercentage() const;
	UFUNCTION(BlueprintCallable, Category = "Mana")
	void RestoreMana(float Amount, bool bIsPassive = true);
	UFUNCTION(BlueprintCallable, Category = "Mana")
	void DrainMana(float Amount);
	UFUNCTION(BlueprintPure, Category = "Mana")
	float GetManaPercentage() const;
	UFUNCTION(BlueprintPure, Category = "Mana")
	float GetMana() const;
	UFUNCTION(BlueprintImplementableEvent, Category = "Healing")
	void SpawnHealthManaText(float Damage, float FontScale, FLinearColor Color);

	// Casting
	UFUNCTION(BlueprintCallable, Category = "Casting")
	virtual void StartCasting(float CastDuration);
	UFUNCTION(BlueprintCallable, Category = "Casting")
	virtual bool InterruptCasting();
	UFUNCTION(BlueprintCallable, Category = "Casting")
	virtual void CastSuccesfull();

	// Locking
	UFUNCTION(BlueprintCallable, Category = "Lock")
	void LockCharacter(float LockDuration);
	UFUNCTION(BlueprintCallable, Category = "Lock")
	void UnlockCharacter();
	UFUNCTION(BlueprintCallable, Category = "Lock")
	void InterruptLock();

	// Stunning
	UFUNCTION(BlueprintCallable, Category = "Stun")
	void Stun(float StunDuration, bool OverrideStun = true);
	UFUNCTION(BlueprintCallable, Category = "Stun")
	void InterruptStun();

	// Abilities
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void UsePotion(EPotion Key);
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	virtual void UseDefensiveSpell(EDefensiveSpell Key, FTransform SpawnTransform);
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	virtual void UseOffensiveSpell(EOffensiveSpell Key, FTransform SpawnTransform);
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void UsePhysicalAttack(EPhysicalAttack Key);
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void BroadcastCounterStrike();

public:
	// Game instance ref (Safe to use since game instance are never destroyed)
	UPROPERTY(BlueprintReadOnly, Category = "Game Instance")
	UTakeMeHomeGameInstance *GameInstance = nullptr;

	// Particles
	UPROPERTY(Category = "Particles", VisibleAnywhere, BlueprintReadOnly)
	UParticleSystemComponent *StunParticle = nullptr;
	UPROPERTY(Category = "Particles", VisibleAnywhere, BlueprintReadOnly)
	UParticleSystemComponent *CastParticle = nullptr;

	// Delegates'
	UPROPERTY(BlueprintAssignable)
	FOnDeathDelegate OnDeathDelegate;
	UPROPERTY(BlueprintAssignable)
	FOnCastingFinishedDelegate OnCastingStatusChange;
	UPROPERTY(BlueprintAssignable)
	FOnInterruptLockDelegate OnLockInterrupted;
	UPROPERTY(BlueprintAssignable)
	FOnCounterStrikeActivate OnCounterStrikeActivated;

	// Timer handles
	FTimerHandle CastTimerHandle;
	FTimerHandle LockTimerHandle;
	FTimerHandle StunTimerHandle;

	UPROPERTY(BlueprintReadWrite, Category = "Element")
	EElement ActiveElement = EElement::E_Fire;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Element")
	EElement ElementType = EElement::E_Fire;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	float PassiveHealthRegenPerSecond = 1.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mana")
	float PassiveManaRegenPerSecond = 1.0f;
	UPROPERTY(BlueprintReadWrite, Category = "Restrictions")
	bool bCanMove = true;
	UPROPERTY(BlueprintReadWrite, Category = "Death")
	bool bIsDead = false;
	UPROPERTY(BlueprintReadWrite, Category = "Restrictions")
	bool bCanUseSpell = true;
	UPROPERTY(BlueprintReadOnly, Category = "Restrictions")
	bool bIsCasting = false;
	UPROPERTY(BlueprintReadOnly, Category = "Restrictions")
	bool bIsLocked = false;
	UPROPERTY(BlueprintReadOnly, Category = "Restrictions")
	bool bIsStunned = false;
	UPROPERTY(BlueprintReadWrite, Category = "Counter Strike")
	bool bCounterStrikeActive = false;
	UPROPERTY(BlueprintReadWrite, Category = "Direction")
	FVector PlayerDirection;

protected:
	// Health/Mana
	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float MaxHealth = 100.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float CurrentHealth;
	UPROPERTY(EditDefaultsOnly, Category = "Mana")
	float MaxMana = 100.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Mana")
	float CurrentMana;
	// Casting
	float TimeCastingBegan = 0.0f;
	float TimeCastingEnds = 0.0f;
};
