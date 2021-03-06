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
	E_Neutral			UMETA(DisplayName = "Neutral"),
	E_Fire 				UMETA(DisplayName = "Fire"),
	E_Water				UMETA(DisplayName = "Water"),
	E_Nature 			UMETA(DisplayName = "Nature"),
};

// Character types
UENUM(BlueprintType)
enum class ECharacterType : uint8
{
	CT_Umir 				UMETA(DisplayName = "Umir"),
	CT_Verdant 				UMETA(DisplayName = "Verdant"),
	CT_Enemy				UMETA(DisplayName = "Enemy"),
};

// Defensive spells
UENUM(BlueprintType)
enum class EDefensiveSpell : uint8
{
	DS_None					UMETA(DisplayName = "None"),
	DS_Spirit_Walk 			UMETA(DisplayName = "Spirit Walk"),
	DS_Star_Shield 			UMETA(DisplayName = "Star Shield"),
	DS_Counter_Strike		UMETA(DisplayName = "Counter Strike"),
};

// Offensive spells
UENUM(BlueprintType)
enum class EOffensiveSpell : uint8
{
	OS_None					UMETA(DisplayName = "None"),
	OS_Tornado 				UMETA(DisplayName = "Tornado"),
	OS_Starfall 			UMETA(DisplayName = "Starfall"),
	OS_Force_Push			UMETA(DisplayName = "Force Push"),
	OS_Liquid_Death			UMETA(DisplayName = "Liquid Death"),
	OS_Vaccum				UMETA(DisplayName = "Vaccum"),
	OS_Death_Plant			UMETA(DisplayName = "Death Plant"),
	OS_Meteor				UMETA(DisplayName = "Meteor"),
	OS_Jump_Attack			UMETA(DisplayName = "Jump Attack"),
	OS_Force_Blast			UMETA(DisplayName = "Force Blast"),
	OS_Death_Ray			UMETA(DisplayName = "Death Ray"),
	OS_Mass_Impale			UMETA(DisplayName = "Mass Impale"),
};

// Potions
UENUM(BlueprintType)
enum class EPotion : uint8
{
	P_None							UMETA(DisplayName = "None"),
	P_Healing_Potion 				UMETA(DisplayName = "Healing Potion"),
	P_Mana_Potion 					UMETA(DisplayName = "Mana Potion"),
	P_Fire_Elemental_Potion			UMETA(DisplayName = "Fire Elemental Potion"),
	P_Nature_Elemental_Potion		UMETA(DisplayName = "Nature Elemental Potion"),
	P_Water_Elemental_Potion		UMETA(DisplayName = "Water Elemental Potion"),
	P_Earth_Elemental_Potion		UMETA(DisplayName = "Earth Elemental Potion"),
	P_Lightning_Elemental_Potion	UMETA(DisplayName = "Lightning Elemental Potion"),
};

// Physical attacks
UENUM(BlueprintType)
enum class EPhysicalAttack : uint8
{
	PA_None					UMETA(DisplayName = "None"),
	PA_Fast_Attack			UMETA(DisplayName = "Fast Attack"),
	PA_Slow_Attack			UMETA(DisplayName = "Slow Attack"),
	PA_Enemy_Attack			UMETA(DisplayName = "Enemy Attack"),
	PA_HW_Attack			UMETA(DisplayName = "HW Attack"),
};

// Decal types
UENUM(BlueprintType)
enum class EDecalType : uint8
{
	DT_None					UMETA(DisplayName = "None"),
	DT_Spell_Circle			UMETA(DisplayName = "Spell Circle"),
	DT_Arrow  				UMETA(DisplayName = "Arrow"),
	DT_Box_Indicator		UMETA(DisplayName = "Box Indicator"),
};

// Decal types
UENUM(BlueprintType)
enum class EAbilityType : uint8
{
	AT_Offensive_Spell		UMETA(DisplayName = "Offensive Spell"),
	AT_Defensive_Spell		UMETA(DisplayName = "Defensive Spell"),
	AT_Physical_Attack 		UMETA(DisplayName = "Physical Attack"),
	AT_Potion				UMETA(DisplayName = "Potion"),
};

// Decal types
UENUM(BlueprintType)
enum class EItem : uint8
{
	I_None					UMETA(DisplayName = "None"),
	I_Empty					UMETA(DisplayName = "Empty"),
	I_Item_1				UMETA(DisplayName = "Item 1"),
	I_Item_2				UMETA(DisplayName = "Item 2"),
	I_Item_3				UMETA(DisplayName = "Item 3"),
	I_Item_4				UMETA(DisplayName = "Item 4"),
	I_Tornado				UMETA(DisplayName = "Tornado"),
	I_Vaccum				UMETA(DisplayName = "Vaccum"),
	I_Force_Push			UMETA(DisplayName = "Force Push"),
	I_Liquid_Death			UMETA(DisplayName = "Liquid Death"),
	I_Death_Plant			UMETA(DisplayName = "Death Plant"),
	I_Starfall				UMETA(DisplayName = "Starfall"),
	I_Spirit_Walk			UMETA(DisplayName = "Spirit Walk"),
	I_Counter_Strike		UMETA(DisplayName = "Counter Strike"),
	I_Star_Shield			UMETA(DisplayName = "Star Shield"),
	I_Healing_Potion		UMETA(DisplayName = "Healing Potion"),
	I_Mana_Potion			UMETA(DisplayName = "Mana Potion"),
	I_Fire_Potion			UMETA(DisplayName = "Fire Potion"),
	I_Water_Potion			UMETA(DisplayName = "Water Potion"),
	I_Nature_Potion			UMETA(DisplayName = "Nature Potion"),
	I_Gold					UMETA(DisplayName = "Gold"),
	I_GodPower				UMETA(DisplayName = "God Power"),
};

// Decal types
UENUM(BlueprintType)
enum class EQuality : uint8
{
	Q_Poor			UMETA(DisplayName = "Poor"),
	Q_Common		UMETA(DisplayName = "Common"),
	Q_Uncommon		UMETA(DisplayName = "Uncommon"),
	Q_Rare			UMETA(DisplayName = "Rare"),
	Q_Epic			UMETA(DisplayName = "Epic"),
};

// Decal types
UENUM(BlueprintType)
enum class ETextWarningLevel : uint8
{
	TWL_Good			UMETA(DisplayName = "Good"),
	TWL_Bad				UMETA(DisplayName = "Bad"),
	TWL_Indifferent		UMETA(DisplayName = "Indifferent"),
};

// Empty class to show in editor
UCLASS()
class UTakeMeHomeEnums : public UUserDefinedEnum
{
	GENERATED_BODY()
};
