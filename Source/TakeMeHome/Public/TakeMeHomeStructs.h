// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TakeMeHomeEnums.h"
#include "Engine/UserDefinedStruct.h"
#include "TakeMeHomeStructs.generated.h"

class AOffensiveSpellBase;
class APotionBase;
class APhysicalAttackBase;
class ADefensiveSpellBase;

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
	EOffensiveSpell Key = EOffensiveSpell::OS_None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	EElement ElementType = EElement::E_Neutral;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FText Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	TSubclassOf<AOffensiveSpellBase> ClassRef;
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
	EDecalType DecalType = EDecalType::DT_None;
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
	EDefensiveSpell Key = EDefensiveSpell::DS_None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FText Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	TSubclassOf<ADefensiveSpellBase> ClassRef;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float Damage = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float LockTime = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float Cooldown = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool bMustActivate = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	EDecalType DecalType = EDecalType::DT_None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float DecalRadius;
};

USTRUCT(BlueprintType)
struct FPhysicalAttack
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	UTexture2D *Icon = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FText Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	EPhysicalAttack Key = EPhysicalAttack::PA_None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FText Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	TSubclassOf<APhysicalAttackBase> ClassRef;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float Damage = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	int32 StaminaCost = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float LockTime = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float Delay = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float StunDuration = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool bDisableMovementWhileAttacking = true;
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
	EPotion Key = EPotion::P_None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FText Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	TSubclassOf<APotionBase> ClassRef;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	int32 Quantity = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	int32 MaxQuantity = 20;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float Cooldown = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float LockTime = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float BuffDurationInMin = 10.0f;
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
	EItem Key = EItem::I_None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FText Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	int32 Value = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	int32 Quantity = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	int32 MaxQuantity = 20;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	EQuality Quality = EQuality::Q_Poor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	TSubclassOf<AActor> ClassRef;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool bAutoUseWhenAttained = false;
};

// Empty class to show in editor
UCLASS()
class UTakeMeHomeStructs : public UUserDefinedStruct
{
	GENERATED_BODY()
};