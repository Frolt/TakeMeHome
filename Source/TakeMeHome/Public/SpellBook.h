// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TakeMeHomeEnums.h"
#include "SpellBook.generated.h"

class ASpellBase;


UCLASS( Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TAKEMEHOME_API USpellBook : public UActorComponent
{
	GENERATED_BODY()

public:	
	USpellBook();

public:
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	TMap<EDefensiveSpell, TSubclassOf<ASpellBase>> DefensiveSpellClasses;
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	TMap<EOffensiveSpell, TSubclassOf<ASpellBase>> OffensiveSpellClasses;
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	TMap<ENormalAttack, TSubclassOf<ASpellBase>> NormalAttackClasses;
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	TMap<EPotion, TSubclassOf<ASpellBase>> PotionClasses;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	TArray<TSubclassOf<ASpellBase>> TestArray;

};
