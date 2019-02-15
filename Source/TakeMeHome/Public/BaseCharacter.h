// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TakeMeHomeEnums.h"
#include "BaseCharacter.generated.h"

class UTakeMeHomeGameInstance;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCastingFinishedDelegate, bool, bWasSuccessful);

UCLASS()
class TAKEMEHOME_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseCharacter();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	// Events
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	UFUNCTION()
	void OnNPCDeath();

	// Health/Mana
	UFUNCTION(BlueprintCallable, Category = "Health")
	void Heal(float Amount);
	UFUNCTION(BlueprintCallable, Category = "Mana")
	void DrainMana(float Amount);
	UFUNCTION(BlueprintCallable, Category = "Mana")
	void RestoreMana(float Amount);
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealthPercentage() const;
	UFUNCTION(BlueprintPure, Category = "Mana")
	float GetManaPercentage() const;

	// Casting
	UFUNCTION(BlueprintCallable, Category = "Casting")
	virtual void StartCasting(float CastDuration);
	UFUNCTION(BlueprintCallable, Category = "Casting")
	virtual bool InterruptCasting();
	UFUNCTION(BlueprintCallable, Category = "Casting")
	virtual void CastSuccesfull();

	// Locking
	UFUNCTION(BlueprintCallable, Category = "Restore")
	void LockCharacter();
	UFUNCTION(BlueprintCallable, Category = "Restore")
	void RestoreCharacter();

	// Abilities
	void UsePotion(EPotion Key);
	void UseDefensiveSpell(EDefensiveSpell Key);
	void UseOffensiveSpell(EOffensiveSpell Key);
	void UsePhysicalAttack(EPhysicalAttack Key);

public:
	// Game instance ref (Safe to use since game instance are never destroyed)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Instance")
	UTakeMeHomeGameInstance *GameInstance = nullptr;

	// Delegates'
	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnDeathDelegate OnDeathDelegate;
	FOnCastingFinishedDelegate OnCastingStatusChange;

	// Timer handles
	FTimerHandle CastTimerHandle;

	UPROPERTY(BlueprintReadWrite, Category = "Element")
	EElement ActiveElement = EElement::E_Fire;
	UPROPERTY(BlueprintReadWrite, Category = "Element")
	EElement ElementType = EElement::E_Fire;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	float MaxHealth = 100.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	float CurrentHealth;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	float MaxMana = 100.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	float CurrentMana;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	float PassiveHealthRegenPerSecond = 1.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	float PassiveManaRegenPerSecond = 1.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	float TimeCastingBegan = 0.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	float TimeCastingEnds = 0.0f;
	UPROPERTY(BlueprintReadWrite, Category = "Umir Controller")
	bool bCanMove = true;
	UPROPERTY(BlueprintReadWrite, Category = "Umir Controller")
	bool bIsDead = false;
	UPROPERTY(BlueprintReadOnly, Category = "Umir Controller")
	bool bIsCasting = false;
	UPROPERTY(BlueprintReadWrite, Category = "Umir Controller")
	bool bCanUseSpell = true;

};
