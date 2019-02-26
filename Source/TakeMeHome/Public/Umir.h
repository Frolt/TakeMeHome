// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "TakeMeHomeEnums.h"
#include "TakeMeHomeStructs.h"
#include "Umir.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UAbilities;
class AOffensiveSpellBase;
class UUserDefinedStruct;
class UTakeMeHomeGameInstance;
class UInventory;
class USoundBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAbilityUsed, bool, bWasInstaCast);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRemoveHighlight);

UCLASS()
class TAKEMEHOME_API AUmir : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AUmir();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Input methods
	void StartJumping();
	void MoveForward(float NormalizedRate);
	void MoveRight(float NormalizedRate);
	void LookRight(float NormalizedRate);
	void LookUp(float NormalizedRate);
	void LookRightRate(float Rate);
	void LookUpRate(float Rate);
	void Zoom(float NormalizedRate);
	void UseFastAttackOrCastSpell();
	void UseSlowAttackOrCancel();

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
	void SetDecalSize(float Size = 256.0f);

	void SetDecalScale(float Y = 1.0f, float Z = 1.0f);

	// Death event
	virtual void OnDeath() override;
	UFUNCTION(BlueprintImplementableEvent, Category = "Death Camera")
	void PlayDeathCamera();

	// Using abilities
	virtual void UseOffensiveSpell(EOffensiveSpell SpellKey, FTransform SpawnTransform) override;
	virtual void UseDefensiveSpell(EDefensiveSpell Key, FTransform SpawnTransform) override;
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool CancelActivatedSpell();
	UFUNCTION(BlueprintCallable, Category = "Restore")
	void EnterCastMode();
	void ResetMousePos();

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

	// HUD functions
	UFUNCTION(BlueprintImplementableEvent, Category = "Warning")
	void NotEnoughMana();
	UFUNCTION(BlueprintImplementableEvent, Category = "Warning")
	void IsOnCooldown();
	UFUNCTION(BlueprintImplementableEvent, Category = "Warning")
	void PotionsAreFull();	
	UFUNCTION(BlueprintImplementableEvent, Category = "Warning")
	void NoMorePotions();
	UFUNCTION(BlueprintImplementableEvent, Category = "Casting Bar")
	void NeedToBeOnGround();
	UFUNCTION(BlueprintImplementableEvent, Category = "Casting Bar")
	void CastingBarActivated();
	UFUNCTION(BlueprintImplementableEvent, Category = "Casting Bar")
	void CastingBarInterrupted();
	UFUNCTION(BlueprintImplementableEvent, Category = "Casting Bar")
	void CastingBarSucceded();
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void LeavingCombat();

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

	// Casting/interrupt/stun
	virtual void StartCasting(float CastDuration) override;
	virtual bool InterruptCasting() override;
	virtual void CastSuccesfull() override;
	virtual void Stun(float StunDuration) override;
	UFUNCTION(BlueprintPure, Category = "Casting")
	float GetCastingPercentage() const;

	// Combat
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void EnteredCombat();
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void LeftCombat();
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void RegisterAggro();
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void DeRegisterAggro();
	UFUNCTION(BlueprintPure, Category = "Combat")
	bool IsInCombat();
	virtual void PassiveRegen(float DeltaTime) override;

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

	// Delegates
	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FOnAbilityUsed OnDefensiveCast;
	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FOnAbilityUsed OnOffensive1Cast;
	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FOnAbilityUsed OnOffensive2Cast;
	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FOnAbilityUsed OnOffensive3Cast;
	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FOnAbilityUsed OnPotionCast;
	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FOnRemoveHighlight OnRemoveHighlight;

	// Timers
	FTimerHandle ResetMouseTimer;

	// Active states
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Active State")
	EOffensiveSpell ActivatedOffensiveSpell = EOffensiveSpell::OS_None;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Active State")
	EDefensiveSpell ActivatedDefensiveSpell = EDefensiveSpell::DS_None;
	UPROPERTY(BlueprintReadWrite, Category = "Active State")
	EDecalType ActiveDecal = EDecalType::DT_None;

	// Action bar
	// ---------------------------------------------------------------------------------------------------------
	UPROPERTY(BlueprintReadWrite, Category = "Spells")
	EDefensiveSpell DefensiveSpellBound = EDefensiveSpell::DS_Spirit_Walk;
	UPROPERTY(BlueprintReadWrite, Category = "Spells")
	EOffensiveSpell OffensiveSpell1Bound = EOffensiveSpell::OS_Force_Push;
	UPROPERTY(BlueprintReadWrite, Category = "Spells")
	EOffensiveSpell OffensiveSpell2Bound = EOffensiveSpell::OS_Starfall;
	UPROPERTY(BlueprintReadWrite, Category = "Spells")
	EOffensiveSpell OffensiveSpell3Bound = EOffensiveSpell::OS_Liquid_Death;
	UPROPERTY(BlueprintReadWrite, Category = "Spells")
	EPotion PotionBound = EPotion::P_Healing_Potion;
	float LastTimeActivatedDefensiveSpell = 0.0f;
	float LastTimeActivatedOffensiveSpell1 = 0.0f;
	float LastTimeActivatedOffensiveSpell2 = 0.0f;
	float LastTimeActivatedOffensiveSpell3 = 0.0f;
	float LastTimeActivatedPotion = 0.0f;
	// ---------------------------------------------------------------------------------------------------------

public:
	UPROPERTY(BlueprintReadWrite, Category = "Umir Controller")
	bool bCanMoveCamera = true;
	UPROPERTY(BlueprintReadWrite, Category = "Umir Controller")
	bool bCanZoom = true;
	UPROPERTY(EditDefaultsOnly, Category = "Umir Controller")
	float MaxTraceDistance = 100000.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float BaseTurnRate = 45.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float BaseLookUpRate = 45.0f;
	UPROPERTY(BlueprintReadWrite, Category = "Aggro")
	int32 AggroCount = 0;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Umir Controller")
	float MaxZoom = 1000.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Umir Controller")
	float MinZoom = 300.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Umir Controller")
	float ZoomStrength = 50.0f;
	bool bShouldResetMouse = false;

};
