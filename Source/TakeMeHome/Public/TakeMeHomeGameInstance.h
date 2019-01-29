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
	// All abilites and spells
	// -------------------------------------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "setup")
	TMap<EOffensiveSpell, FOffensiveSpell> OffensiveSpells;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "setup")
	TMap<EDefensiveSpell, FDefensiveSpell> DefensiveSpells;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "setup")
	TMap<ENormalAttack, FNormalAttack> NormalAttacks;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "setup")
	TMap<EPotion, FPotion> Potions;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "setup")
	TMap<EItem, FItem> Items;
	// -------------------------------------------------------------------------------------

	// All items
	// TODO
};
