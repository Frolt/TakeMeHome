// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TakeMeHomeEnums.h"
#include "TakeMeHomeStructs.h"
#include "Abilities.generated.h"

class ASpellBase;
class UTakeMeHomeGameInstance;


UCLASS( Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TAKEMEHOME_API UAbilities : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAbilities();
	virtual void BeginPlay() override;

	// Add/Remove
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void AddDefensive(EDefensiveSpell DefensiveSpell);
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void AddOffensive(EOffensiveSpell OffensiveSpell);
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void AddPhysical(EPhysicalAttack PhysicalAttack);
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool AddPotion(EPotion Potion);
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool RemoveDefensive(EDefensiveSpell Key);
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool RemoveOffensive(EOffensiveSpell Key);
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool RemovePhysical(EPhysicalAttack Key);
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool RemovePotion(EPotion Potion);

	UFUNCTION(BlueprintPure, Category = "Potion")
	int32 GetPotionsLeft(EPotion Key) const;


public:
	// Game instance ref (Safe to use since game instance are never destroyed)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Instance")
	UTakeMeHomeGameInstance *GameInstance = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Spells")
	TArray<FOffensiveSpell> OffensiveSpells;
	UPROPERTY(BlueprintReadOnly, Category = "Spells")
	TArray<FDefensiveSpell> DefensiveSpells;
	UPROPERTY(BlueprintReadOnly, Category = "Spells")
	TArray<FPhysicalAttack> PhysicalAttacks;
	UPROPERTY(BlueprintReadOnly, Category = "Spells")
	TArray<FPotion> Potions;

};
