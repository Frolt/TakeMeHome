// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "TakeMeHomeEnums.h"
#include "TakeMeHomeStructs.h"
#include "TakeMeHomeGameInstance.generated.h"

class UParticleSystemComponent;
class UParticleSystem;

/**
 * 
 */
UCLASS()
class TAKEMEHOME_API UTakeMeHomeGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	// Getters
	FDefensiveSpell *GetDefensiveSpell(EDefensiveSpell Key);
	FOffensiveSpell *GetOffensiveSpell(EOffensiveSpell Key);
	FPhysicalAttack *GetPhysicalAttack(EPhysicalAttack Key);
	FPotion *GetPotion(EPotion Key);
	FItem *GetItem(EItem Key);
	
public:
	// Abilites 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "setup")
	TMap<EOffensiveSpell, FOffensiveSpell> OffensiveSpells;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "setup")
	TMap<EDefensiveSpell, FDefensiveSpell> DefensiveSpells;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "setup")
	TMap<EPhysicalAttack, FPhysicalAttack> PhysicalAttacks;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "setup")
	TMap<EPotion, FPotion> Potions;

	// Items
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "setup")
	TMap<EItem, FItem> Items;

	// Damage type class refs
	UPROPERTY(EditDefaultsOnly, Category = "Damage Type Refs")
	TSubclassOf<UDamageType> FireDamage;
	UPROPERTY(EditDefaultsOnly, Category = "Damage Type Refs")
	TSubclassOf<UDamageType> NatureDamage;
	UPROPERTY(EditDefaultsOnly, Category = "Damage Type Refs")
	TSubclassOf<UDamageType> WaterDamage;
	UPROPERTY(EditDefaultsOnly, Category = "Damage Type Refs")
	TSubclassOf<UDamageType> EarthDamage;
	UPROPERTY(EditDefaultsOnly, Category = "Damage Type Refs")
	TSubclassOf<UDamageType> LightningDamage;

	// Character particle effects
	UPROPERTY(EditDefaultsOnly, Category = "Particles")
	UParticleSystem *StunParticle = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Particles")
	UParticleSystem *CastingParticle= nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Particles")
	UParticleSystem *HealingParticle = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Particles")
	UParticleSystem *ManaParticle = nullptr;

	// Colors
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Colors")
	FLinearColor DefensiveColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Colors")
	FLinearColor OffensiveColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Colors")
	FLinearColor PhysicalColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Colors")
	FLinearColor PotionColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Status Colors")
	FLinearColor HealthColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Status Colors")
	FLinearColor ManaColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Element Colors")
	FLinearColor NeutralColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Element Colors")
	FLinearColor FireColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Element Colors")
	FLinearColor WaterColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Element Colors")
	FLinearColor NatureColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Element Colors")
	FLinearColor EarthColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Element Colors")
	FLinearColor LightningColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quality Colors")
	FLinearColor PoorColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quality Colors")
	FLinearColor CommonColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quality Colors")
	FLinearColor UncommonColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quality Colors")
	FLinearColor RareColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quality Colors")
	FLinearColor EpicColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HUD Message Colors")
	FLinearColor BadColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HUD Message Colors")
	FLinearColor IndifferentColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HUD Message Colors")
	FLinearColor GoodColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage Text Colors")
	FLinearColor NotVeryEffectiveColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage Text Colors")
	FLinearColor NormalColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage Text Colors")
	FLinearColor SuperEffectiveColor;
};
