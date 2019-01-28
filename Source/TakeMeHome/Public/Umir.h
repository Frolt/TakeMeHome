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

UCLASS()
class TAKEMEHOME_API AUmir : public ACharacter
{
	GENERATED_BODY()

public:
	AUmir();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/// Input methods
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

	/// Others
	void ShowDecalAtMousePosInWorld();
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealthPercentage() const;
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetManaPercentage() const;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent *CameraBoom = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent *FollowCamera = nullptr;
	UPROPERTY(BlueprintReadWrite, Category = "Setup")
	UDecalComponent *SpellCircle = nullptr;

	// Spell bar TEST // TODO might make struct later
	// ---------------------------------------------------------------------------------------------------------
	EDefensiveSpell DefensiveSpellActive = EDefensiveSpell::E_None;
	EOffensiveSpell OffensiveSpellActive1 = EOffensiveSpell::E_Tornado;
	EOffensiveSpell OffensiveSpellActive2 = EOffensiveSpell::E_Starfall;
	EOffensiveSpell OffensiveSpellActive3 = EOffensiveSpell::E_Lightning_Bolt;
	EPotion PotionActive = EPotion::E_None;
	// ---------------------------------------------------------------------------------------------------------

	// Aquired abilities
	// ---------------------------------------------------------------------------------------------------------
	UPROPERTY(BlueprintReadOnly, Category = "Spells")
	TMap<EOffensiveSpell, FOffensiveSpell> AquiredOffensiveSpells;
	UPROPERTY(BlueprintReadOnly, Category = "Spells")
	TMap<EDefensiveSpell, FDefensiveSpell> AquiredDefensiveSpells;
	UPROPERTY(BlueprintReadOnly, Category = "Spells")
	TMap<ENormalAttack, FNormalAttack> AquiredNormalAttacksSpells;
	UPROPERTY(BlueprintReadOnly, Category = "Spells")
	TMap<EPotion, FPotion> AquiredPotions;
	// ---------------------------------------------------------------------------------------------------------

	// Game instance ref (Safe to use since game instance are never destroyed)
	UTakeMeHomeGameInstance *GameInstance = nullptr;

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
