// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedEnum.h"
#include "TakeMeHomeEnums.generated.h"

/**
 * Contains all enums for the game
 */

// Elements
UENUM(BlueprintType)
enum class EElement : uint8
{
	E_None				UMETA(DisplayName = "None"),
	E_Fire 				UMETA(DisplayName = "Fire"),
	E_Nature 			UMETA(DisplayName = "Nature"),
	E_Water				UMETA(DisplayName = "Water"),
	E_Earth				UMETA(DisplayName = "Earth"),
	E_Lightning			UMETA(DisplayName = "Lightning"),
};

// Character types
UENUM(BlueprintType)
enum class ECharacterType : uint8
{
	E_None				UMETA(DisplayName = "None"),
	E_Umir 				UMETA(DisplayName = "Umir"),
	E_Verdant 			UMETA(DisplayName = "Verdant"),
	E_Enemy				UMETA(DisplayName = "Enemy"),
};

// Defensive spells
UENUM(BlueprintType)
enum class EDefensiveSpell : uint8
{
	E_None					UMETA(DisplayName = "None"),
	E_Spirit_Walk 			UMETA(DisplayName = "Spirit Walk"),
	E_Star_Shield 			UMETA(DisplayName = "Star Shield"),
	E_Counter_Strike		UMETA(DisplayName = "Counter Strike"),
};

// Offensive spells
UENUM(BlueprintType)
enum class EOffensiveSpell : uint8
{
	E_None					UMETA(DisplayName = "None"),
	E_Tornado 				UMETA(DisplayName = "Tornado"),
	E_Starfall 				UMETA(DisplayName = "Starfall"),
	E_Force_Push			UMETA(DisplayName = "Force Push"),
	E_Lightning_Bolt		UMETA(DisplayName = "Lightning Bolt"),
};

// Potions
UENUM(BlueprintType)
enum class EPotion : uint8
{
	E_None							UMETA(DisplayName = "None"),
	E_Healing_Potion 				UMETA(DisplayName = "Healing Potion"),
	E_Mana_Potion 					UMETA(DisplayName = "Mana Potion"),
	E_Fire_Elemental_Potion			UMETA(DisplayName = "Fire Elemental Potion"),
	E_Nature_Elemental_Potion		UMETA(DisplayName = "Nature Elemental Potion"),
	E_Water_Elemental_Potion		UMETA(DisplayName = "Water Elemental Potion"),
	E_Earth_Elemental_Potion		UMETA(DisplayName = "Earth Elemental Potion"),
	E_Lightning_Elemental_Potion	UMETA(DisplayName = "Lightning Elemental Potion"),
};

// Normal attacks
UENUM(BlueprintType)
enum class ENormalAttack : uint8
{
	E_None					UMETA(DisplayName = "None"),
	E_Melee_Attack_1		UMETA(DisplayName = "Melee Attack 1"),
	E_Melee_Attack_2		UMETA(DisplayName = "Melee Attack 2"),
	E_Melee_Attack_3		UMETA(DisplayName = "Melee Attack 3"),
};

// Decal types
UENUM(BlueprintType)
enum class EDecalType : uint8
{
	E_None					UMETA(DisplayName = "None"),
	E_Spell_Circle			UMETA(DisplayName = "Spell Circle"),
	E_Arrow  				UMETA(DisplayName = "Arrow"),
	E_Box_Indicator			UMETA(DisplayName = "Box Indicator"),
};

// Decal types
UENUM(BlueprintType)
enum class EAbilityType : uint8
{
	E_None					UMETA(DisplayName = "None"),
	E_Offensive_Spell		UMETA(DisplayName = "Offensive Spell"),
	E_Defensive_Spell		UMETA(DisplayName = "Defensive Spell"),
	E_Normal_Attack 		UMETA(DisplayName = "Normal Attack"),
	E_Potion				UMETA(DisplayName = "Potion"),
};

// Decal types
UENUM(BlueprintType)
enum class EItem : uint8
{
	E_None			UMETA(DisplayName = "None"),
	E_Item_1		UMETA(DisplayName = "Item 1"),
	E_Item_2		UMETA(DisplayName = "Item 2"),
	E_Item_3		UMETA(DisplayName = "Item 3"),
	E_Item_4		UMETA(DisplayName = "Item 4"),
};

// Empty class to show in editor
UCLASS()
class UTakeMeHomeEnums : public UUserDefinedEnum
{
	GENERATED_BODY()
};
