// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TakeMeHomeEnums.h"
#include "TakeMeHomeStructs.h"
#include "Umir.generated.h"

class USpringArmComponent;
class UCameraComponent;
class AStarfall;
class AForcePush;
class ATornado;
class ALightningBolt;
class UAbilities;
class ASpellBase;
class UUserDefinedStruct;
class UTakeMeHomeGameInstance;
class UInventory;

UCLASS()
class TAKEMEHOME_API AUmir : public ACharacter
{
	GENERATED_BODY()

public:
	AUmir();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Input methods
	void MoveForward(float NormalizedRate);
	void MoveRight(float NormalizedRate);
	void LookRight(float NormalizedRate);
	void LookUp(float NormalizedRate);
	void LookRightRate(float Rate);
	void LookUpRate(float Rate);
	void Zoom(float NormalizedRate);
	void ActivatePhysical1OrCastSpell();
	void ActivatePhysical2OrCancel();

	// Activate action bar
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void ActivateOffensiveSlot1();
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void ActivateOffensiveSlot2();
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void ActivateOffensiveSlot3();
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void ActivateDefensiveSlot();
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void ActivatePotionSlot();

	// Decal functions
	void MoveDecalToMouseHitLocation();
	void RotateDecalAroundPlayer();

	// Restore
	void RestoreMovement();
	void ResetMousePos();

	// Using abilities
	void CastOffensiveSpell(EOffensiveSpell SpellKey);
	void CastDefensiveSpell(EDefensiveSpell SpellKey);
	void UsePhysicalAttack1();
	void UsePhysicalAttack2();
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool CancelActivatedSpell();

	// Health/Mana
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealthPercentage() const;
	UFUNCTION(BlueprintPure, Category = "Mana")
	float GetManaPercentage() const;

	// Cooldown functions
	UFUNCTION(BlueprintPure, Category = "Cooldown")
	float GetDefensiveSlotCooldown() const;
	UFUNCTION(BlueprintPure, Category = "Cooldown")
	float GetDefensiveSlotCooldownPercentage() const;
	UFUNCTION(BlueprintPure, Category = "Cooldown")
	float GetOffensiveSlot1Cooldown() const;
	UFUNCTION(BlueprintPure, Category = "Cooldown")
	float GetOffensiveSlot1CooldownPercentage() const;
	UFUNCTION(BlueprintPure, Category = "Cooldown")
	float GetOffensiveSlot2Cooldown() const;
	UFUNCTION(BlueprintPure, Category = "Cooldown")
	float GetOffensiveSlot2CooldownPercentage() const;
	UFUNCTION(BlueprintPure, Category = "Cooldown")
	float GetOffensiveSlot3Cooldown() const;
	UFUNCTION(BlueprintPure, Category = "Cooldown")
	float GetOffensiveSlot3CooldownPercentage() const;
	UFUNCTION(BlueprintPure, Category = "Cooldown")
	float GetPotionSlotCooldown() const;
	UFUNCTION(BlueprintPure, Category = "Cooldown")
	float GetPotionSlotCooldownPercentage() const;

	// HUD Warnings
	UFUNCTION(BlueprintImplementableEvent, Category = "Warning")
	void NotEnoughMana();
	UFUNCTION(BlueprintImplementableEvent, Category = "Warning")
	void IsOnCooldown();
	UFUNCTION(BlueprintImplementableEvent, Category = "Warning")
	void PotionsAreFull();

	// Ability binding
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void BindDefensiveSpell(EDefensiveSpell DefensiveSpell);
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void BindOffensiveSpell1(EOffensiveSpell OffensiveSpell);
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void BindOffensiveSpell2(EOffensiveSpell OffensiveSpell);
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void BindOffensiveSpell3(EOffensiveSpell OffensiveSpell);
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void BindPotion(EPotion Potion);


public:
	// Umir's components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent *CameraBoom = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent *FollowCamera = nullptr;
	UPROPERTY(BlueprintReadWrite, Category = "Setup")
	UDecalComponent *Decal = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	UMaterialInterface *SpellCircleMaterial = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	UMaterialInterface *SpellBoxMaterial = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	UMaterialInterface *SpellArrowMaterial = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	UInventory *Inventory = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	UAbilities *Abilities = nullptr;

	// Game instance ref (Safe to use since game instance are never destroyed)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Instance")
	UTakeMeHomeGameInstance *GameInstance = nullptr;

	// Active states
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Active State")
	EOffensiveSpell ActivatedOffensiveSpell = EOffensiveSpell::OS_None;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Active State")
	EDefensiveSpell ActivatedDefensiveSpell = EDefensiveSpell::DS_None;
	UPROPERTY(BlueprintReadWrite, Category = "Active State")
	EElement ActiveElement = EElement::E_Neutral;
	UPROPERTY(BlueprintReadWrite, Category = "Active State")
	EDecalType ActiveDecal = EDecalType::DT_None;

	// Action bar
	// ---------------------------------------------------------------------------------------------------------
	UPROPERTY(BlueprintReadWrite, Category = "Spells")
	EDefensiveSpell DefensiveSpellBound = EDefensiveSpell::DS_None;
	UPROPERTY(BlueprintReadWrite, Category = "Spells")
	EOffensiveSpell OffensiveSpell1Bound = EOffensiveSpell::OS_Tornado;
	UPROPERTY(BlueprintReadWrite, Category = "Spells")
	EOffensiveSpell OffensiveSpell2Bound = EOffensiveSpell::OS_Starfall;
	UPROPERTY(BlueprintReadWrite, Category = "Spells")
	EOffensiveSpell OffensiveSpell3Bound = EOffensiveSpell::OS_Lightning_Bolt;
	UPROPERTY(BlueprintReadWrite, Category = "Spells")
	EPotion PotionBound = EPotion::P_None;
	float LastTimeActivatedDefensiveSpell = 0.0f;
	float LastTimeActivatedOffensiveSpell1 = 0.0f;
	float LastTimeActivatedOffensiveSpell2 = 0.0f;
	float LastTimeActivatedOffensiveSpell3 = 0.0f;
	float LastTimeActivatedPotion = 0.0f;
	// ---------------------------------------------------------------------------------------------------------

public:
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
	UPROPERTY(BlueprintReadWrite, Category = "Damage")
	float LastTimeTookDamage = 0.0f;
	UPROPERTY(BlueprintReadWrite, Category = "Umir Controller")
	bool bStopMovingCamera = false;
	UPROPERTY(BlueprintReadWrite, Category = "Umir Controller")
	bool bStopMovement = false;
	UPROPERTY(BlueprintReadWrite, Category = "Umir Controller")
	bool bStopZooming = false;
	UPROPERTY(BlueprintReadWrite, Category = "Umir Controller")
	bool bCanCastSpell = true;
	UPROPERTY(EditDefaultsOnly, Category = "Umir Controller")
	float MaxTraceDistance = 100000.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float BaseTurnRate = 45.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float BaseLookUpRate = 45.0f;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Umir Controller")
	float MaxZoom = 1000.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Umir Controller")
	float MinZoom = 300.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Umir Controller")
	float ZoomStrength = 50.0f;

};
