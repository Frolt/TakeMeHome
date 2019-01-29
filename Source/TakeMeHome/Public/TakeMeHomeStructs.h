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
	FString Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	EOffensiveSpell Key = EOffensiveSpell::E_None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FString Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	TSubclassOf<ASpellBase> ClassRef;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float ManaCost = 0.0f;
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
	FString Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	EDefensiveSpell Key = EDefensiveSpell::E_None;
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
	FString Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	ENormalAttack Key = ENormalAttack::E_None;
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
	FString Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	EPotion Key = EPotion::E_None;
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
struct FItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FString Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	EItem Key = EItem::E_None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FString Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	TSubclassOf<AActor> ClassRef;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	UTexture2D *Icon = nullptr;

};

// Empty class to show in editor
UCLASS()
class UTakeMeHomeStructs : public UUserDefinedStruct
{
	GENERATED_BODY()
};