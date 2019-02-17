// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "TakeMeHomeEnums.h"
#include "TakeMeHomeStructs.h"
#include "TakeMeHomeGameInstance.generated.h"

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
	
public:
	// All abilites and spells
	// -------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "setup")
	TMap<EOffensiveSpell, FOffensiveSpell> OffensiveSpells;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "setup")
	TMap<EDefensiveSpell, FDefensiveSpell> DefensiveSpells;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "setup")
	TMap<EPhysicalAttack, FPhysicalAttack> PhysicalAttacks;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "setup")
	TMap<EPotion, FPotion> Potions;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "setup")
	TMap<EItem, FItem> Items;
	// -------------------------------------------------------------------------------------

	// All items
	// TODO
};
