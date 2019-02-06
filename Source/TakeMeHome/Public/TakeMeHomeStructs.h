// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TakeMeHomeEnums.h"
#include "Engine/UserDefinedStruct.h"
#include "TakeMeHomeStructs.generated.h"

class ASpellBase;

/**
 * Contains all the structs for items and abilities
 */

USTRUCT(BlueprintType)
struct FOffensiveSpell
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	UTexture2D *Icon = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FText Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	EOffensiveSpell Key = EOffensiveSpell::E_None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	EElement ElementType = EElement::E_Neutral;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FText Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	TSubclassOf<ASpellBase> ClassRef;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float Damage = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float CastTime = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float StunDuration = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float ManaCost = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float Cooldown = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float Range = 1000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool bMustActivate = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	EDecalType DecalType = EDecalType::E_None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float DecalRadius;

};

USTRUCT(BlueprintType)
struct FDefensiveSpell
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	UTexture2D *Icon = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FText Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	EDefensiveSpell Key = EDefensiveSpell::E_None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FText Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	TSubclassOf<ASpellBase> ClassRef;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float Damage = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float CastTime = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float Cooldown = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool bMustActivate = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	EDecalType DecalType = EDecalType::E_None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float DecalRadius;

};

USTRUCT(BlueprintType)
struct FNormalAttack
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	UTexture2D *Icon = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FText Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	ENormalAttack Key = ENormalAttack::E_None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FText Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	TSubclassOf<ASpellBase> ClassRef;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float Cooldown = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float Damage = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float CastTime = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float StunDuration = 0.0f;

};

USTRUCT(BlueprintType)
struct FPotion
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	UTexture2D *Icon = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FText Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	EPotion Key = EPotion::E_None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FText Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	TSubclassOf<ASpellBase> ClassRef;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float Cooldown = 0.0f;

};

USTRUCT(BlueprintType)
struct FItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	UTexture2D *Icon = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FText Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	EItem Key = EItem::E_None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FText Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	int32 Value = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	int32 Quantity = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	int32 MaxQuantity = 20;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	EQuality Quality = EQuality::E_Poor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	TSubclassOf<AActor> ClassRef;

};

// Empty class to show in editor
UCLASS()
class UTakeMeHomeStructs : public UUserDefinedStruct
{
	GENERATED_BODY()
};