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
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void CastOffensiveSpell1();
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void CastOffensiveSpell2();
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void CastOffensiveSpell3();
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void CastDefensiveSpell();
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void UsePotion();

	// Others
	void ShowDecalAtMousePosInWorld();
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealthPercentage() const;
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetManaPercentage() const;

	// Spell/Item utility functions
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent *CameraBoom = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent *FollowCamera = nullptr;
	UPROPERTY(BlueprintReadWrite, Category = "Setup")
	UDecalComponent *SpellCircle = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	UInventory *Inventory = nullptr;

	// Game instance ref (Safe to use since game instance are never destroyed)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Instance")
	UTakeMeHomeGameInstance *GameInstance = nullptr;


	// Action bar TEST // TODO might make struct later
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
	UPROPERTY(BlueprintReadWrite, Category = "Damage")
	float LastTimeTookDamage = 0.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Umir Controller")
	float MaxTargetRange = 1000000.0f;
	UPROPERTY(BlueprintReadWrite, Category = "Umir Controller")
	bool bShouldLockMouse = true;
	UPROPERTY(BlueprintReadWrite, Category = "Umir Controller")
	bool bStopMovingCamera = false;
	UPROPERTY(BlueprintReadWrite, Category = "Umir Controller")
	bool bIsLeftMouseButtonPressed = false;
	UPROPERTY(BlueprintReadWrite, Category = "Umir Controller")
	bool bIsRightMouseButtonPressed = false;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Umir Controller")
	float MaxZoom = 1000.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Umir Controller")
	float MinZoom = 300.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Umir Controller")
	float ZoomStrength = 50.0f;
	FVector2D PrevMousePos;

};
