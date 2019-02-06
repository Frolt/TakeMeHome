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
class USpellBook;
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
	void Zoom(float NormalizedRate);
	void LeftMouseButtonPressed();
	void LeftMouseButtonReleased();
	void RightMouseButtonPressed();
	void RightMouseButtonReleased();

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

	// Others
	void MoveDecalToMouseHitLocation();
	void RotateDecalAroundPlayer();
	void RestoreMovement();
	void CastOffensiveSpell(EOffensiveSpell SpellKey);
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealthPercentage() const;
	UFUNCTION(BlueprintPure, Category = "Health")
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
	UFUNCTION(BlueprintImplementableEvent, Category = "Casting")
	void NotEnoughMana();
	UFUNCTION(BlueprintImplementableEvent, Category = "Casting")
	void IsOnCooldown();

	// Add/Bind abilities
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void AddDefensiveSpell(EDefensiveSpell DefensiveSpell);
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void AddOffensiveSpell(EOffensiveSpell OffensiveSpell);
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void AddPotion(EPotion Potion);
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
	// Umir components
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

	// Game instance ref (Safe to use since game instance are never destroyed)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Instance")
	UTakeMeHomeGameInstance *GameInstance = nullptr;

	// Active states
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spells")
	EOffensiveSpell ActivatedSpell = EOffensiveSpell::E_None;
	UPROPERTY(BlueprintReadWrite, Category = "Spells")
	EElement ActiveElement = EElement::E_Neutral;
	UPROPERTY(BlueprintReadWrite, Category = "Spells")
	EDecalType ActiveDecal = EDecalType::E_None;

	// Action bar
	// ---------------------------------------------------------------------------------------------------------
	UPROPERTY(BlueprintReadWrite, Category = "Spells")
	EDefensiveSpell DefensiveSpellActive = EDefensiveSpell::E_None;
	UPROPERTY(BlueprintReadWrite, Category = "Spells")
	EOffensiveSpell OffensiveSpellActive1 = EOffensiveSpell::E_Tornado;
	UPROPERTY(BlueprintReadWrite, Category = "Spells")
	EOffensiveSpell OffensiveSpellActive2 = EOffensiveSpell::E_Starfall;
	UPROPERTY(BlueprintReadWrite, Category = "Spells")
	EOffensiveSpell OffensiveSpellActive3 = EOffensiveSpell::E_Lightning_Bolt;
	UPROPERTY(BlueprintReadWrite, Category = "Spells")
	EPotion PotionActive = EPotion::E_None;
	float LastTimeActivatedDefensiveSpell = 0.0f;
	float LastTimeActivatedOffensiveSpell1 = 0.0f;
	float LastTimeActivatedOffensiveSpell2 = 0.0f;
	float LastTimeActivatedOffensiveSpell3 = 0.0f;
	float LastTimeActivatedPotion = 0.0f;
	// ---------------------------------------------------------------------------------------------------------

	// Aquired abilities
	// ---------------------------------------------------------------------------------------------------------
	UPROPERTY(BlueprintReadOnly, Category = "Spells")
	TArray<FOffensiveSpell> AquiredOffensiveSpells;
	UPROPERTY(BlueprintReadOnly, Category = "Spells")
	TArray<FDefensiveSpell> AquiredDefensiveSpells;
	UPROPERTY(BlueprintReadOnly, Category = "Spells")
	TArray<FNormalAttack> AquiredNormalAttacks;
	UPROPERTY(BlueprintReadOnly, Category = "Spells")
	TArray<FPotion> AquiredPotions;
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
	// TODO not using atm
	UPROPERTY(BlueprintReadWrite, Category = "Damage")
	float LastTimeTookDamage = 0.0f;
	UPROPERTY(BlueprintReadWrite, Category = "Umir Controller")
	bool bShouldLockMouse = true;
	UPROPERTY(BlueprintReadWrite, Category = "Umir Controller")
	bool bStopMovingCamera = false;
	UPROPERTY(BlueprintReadWrite, Category = "Umir Controller")
	bool bStopMovement = false;
	UPROPERTY(BlueprintReadWrite, Category = "Umir Controller")
	bool bStopZooming = false;
	UPROPERTY(BlueprintReadWrite, Category = "Umir Controller")
	bool bIsLeftMouseButtonPressed = false;
	UPROPERTY(BlueprintReadWrite, Category = "Umir Controller")
	bool bIsRightMouseButtonPressed = false;
	// Determines line trace length
	UPROPERTY(EditDefaultsOnly, Category = "Umir Controller")
	float MaxTraceDistance = 100000.0f;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Umir Controller")
	float MaxZoom = 1000.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Umir Controller")
	float MinZoom = 300.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Umir Controller")
	float ZoomStrength = 50.0f;
	FVector2D PrevMousePos;

};
