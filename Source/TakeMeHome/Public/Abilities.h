// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TakeMeHomeEnums.h"
#include "Abilities.generated.h"

class ASpellBase;

/**
 * This struct keeps instances of all the spells and abilities in the game
 */
USTRUCT(BlueprintType)
struct FAbilities
{
	GENERATED_BODY()

	// TODO May keep all abilities inside this struct in future
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "setup")
	//TMap<EOffensiveSpell, FOffensiveSpell> OffensiveSpells;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "setup")
	//TMap<EDefensiveSpell, FDefensiveSpell> DefensiveSpells;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "setup")
	//TMap<ENormalAttack, FNormalAttack> NormalAttacks;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "setup")
	//TMap<EPotion, FPotion> Potions;
};

USTRUCT(BlueprintType)
struct FOffensiveSpell
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	EOffensiveSpell SpellName = EOffensiveSpell::E_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	TSubclassOf<ASpellBase> ClassRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	int32 ManaCost = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float Cooldown = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool bMustActivate = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	EDecalType DecalType = EDecalType::E_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float DecalRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	UTexture2D *Icon = nullptr;

};

USTRUCT(BlueprintType)
struct FDefensiveSpell
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	EOffensiveSpell SpellName = EOffensiveSpell::E_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	TSubclassOf<ASpellBase> ClassRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float Cooldown = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool bMustActivate = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	EDecalType DecalType = EDecalType::E_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float DecalRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	UTexture2D *Icon = nullptr;

};

USTRUCT(BlueprintType)
struct FNormalAttack
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	EOffensiveSpell SpellName = EOffensiveSpell::E_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	TSubclassOf<ASpellBase> ClassRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float Cooldown = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	UTexture2D *Icon = nullptr;

};

USTRUCT(BlueprintType)
struct FPotion
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	EOffensiveSpell SpellName = EOffensiveSpell::E_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	TSubclassOf<ASpellBase> ClassRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float Cooldown = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	UTexture2D *Icon = nullptr;

};
