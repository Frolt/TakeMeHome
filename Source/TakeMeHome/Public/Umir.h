// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TakeMeHomeEnums.h"
#include "Abilities.h"
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
	void CastSpell1();
	void CastSpell2();
	void CastSpell3();
	void CastSpell4();
	void HandleEscape();

	/// Others
	void ShowDecalAtMousePosInWorld();
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealthPercentage() const;

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
	EOffensiveSpell OffensiveSpellActive1 = EOffensiveSpell::E_None;
	EOffensiveSpell OffensiveSpellActive2 = EOffensiveSpell::E_None;
	EOffensiveSpell OffensiveSpellActive3 = EOffensiveSpell::E_None;
	EOffensiveSpell OffensiveSpellActive4 = EOffensiveSpell::E_None;
	EPotion PotionActive = EPotion::E_None;
	// ---------------------------------------------------------------------------------------------------------

	// Aquired spells
	UPROPERTY(BlueprintReadOnly, Category = "Spells")
	TMap<EOffensiveSpell, FOffensiveSpell> AquiredOffensiveSpells;

	// Game instance ref (Safe to use since game instance are never destroyed)
	UTakeMeHomeGameInstance *GameInstance = nullptr;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	int32 MaxHealth = 100;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	int32 CurrentHealth;
	UPROPERTY(BlueprintReadWrite, Category = "Damage")
	float LastTimeTookDamage = 0.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Umir Controller")
	float TargetRange = 1000.0f;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Umir Controller")
	float MaxZoom = 1000.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Umir Controller")
	float MinZoom = 300.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Umir Controller")
	float ZoomStrength = 50.0f;

	bool bIsLeftMouseButtonPressed = false;
	bool bIsRightMouseButtonPressed = false;
	FVector2D PrevMousePos;

};
