// Fill out your copyright notice in the Description page of Project Settings.

#include "TakeMeHomeGameInstance.h"
#include "Umir.h"
#include "Abilities.h"
#include "Inventory.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"


FDefensiveSpell *UTakeMeHomeGameInstance::GetDefensiveSpell(EDefensiveSpell Key)
{
	checkf(Key != EDefensiveSpell::DS_None, TEXT("DEFENSIVE SPELL IS NOT CREATED IN GAME INSTANCE"));
	return DefensiveSpells.Find(Key);
}

FOffensiveSpell *UTakeMeHomeGameInstance::GetOffensiveSpell(EOffensiveSpell Key)
{
	checkf(Key != EOffensiveSpell::OS_None, TEXT("OFFENSIVE SPELL IS NOT CREATED IN GAME INSTANCE"));
	return OffensiveSpells.Find(Key);
}

FPhysicalAttack *UTakeMeHomeGameInstance::GetPhysicalAttack(EPhysicalAttack Key)
{
	checkf(Key != EPhysicalAttack::PA_None, TEXT("PHYSICAL ATTACK IS NOT CREATED IN GAME INSTANCE"));
	return PhysicalAttacks.Find(Key);
}

FPotion *UTakeMeHomeGameInstance::GetPotion(EPotion Key)
{
	checkf(Key != EPotion::P_None, TEXT("POTION IS NOT CREATED IN GAME INSTANCE"));
	return Potions.Find(Key);
}

FItem * UTakeMeHomeGameInstance::GetItem(EItem Key)
{
	checkf(Key != EItem::I_None, TEXT("ITEM IS NOT CREATED IN GAME INSTANCE"));
	return Items.Find(Key);
}

void UTakeMeHomeGameInstance::SaveUmir()
{
	auto Umir = Cast<AUmir>(GetWorld()->GetFirstPlayerController()->GetCharacter());
	if (!ensure(Umir)) return;

	// Action bar
	SaveDefensiveSpellBound = Umir->DefensiveSpellBound;
	SaveOffensiveSpell1Bound = Umir->OffensiveSpell1Bound;
	SaveOffensiveSpell2Bound = Umir->OffensiveSpell2Bound;
	SaveOffensiveSpell3Bound = Umir->OffensiveSpell3Bound;
	SavePotionBound = Umir->PotionBound;

	// Abilities
	SaveDefensiveSpells = Umir->Abilities->DefensiveSpells;
	SaveOffensiveSpells = Umir->Abilities->OffensiveSpells;
	SavePhysicalAttacks = Umir->Abilities->PhysicalAttacks;
	SavePotions = Umir->Abilities->Potions;
	
	// Inventory
	SaveItems = Umir->Inventory->Items;
	SaveInventorySize = Umir->Inventory->InventorySize;
	SaveGold = Umir->Inventory->Gold;

}

void UTakeMeHomeGameInstance::LoadUmir()
{
	auto Umir = Cast<AUmir>(GetWorld()->GetFirstPlayerController()->GetCharacter());
	if (!ensure(Umir)) return;

	// Action bar
	Umir->DefensiveSpellBound = SaveDefensiveSpellBound;
	Umir->OffensiveSpell1Bound = SaveOffensiveSpell1Bound;
	Umir->OffensiveSpell2Bound = SaveOffensiveSpell2Bound;
	Umir->OffensiveSpell3Bound = SaveOffensiveSpell3Bound;
	Umir->PotionBound = SavePotionBound;

	// Abilities
	Umir->Abilities->DefensiveSpells = SaveDefensiveSpells;
	Umir->Abilities->OffensiveSpells = SaveOffensiveSpells;
	Umir->Abilities->PhysicalAttacks = SavePhysicalAttacks;
	Umir->Abilities->Potions = SavePotions;

	// Inventory
	Umir->Inventory->Items = SaveItems;
	Umir->Inventory->InventorySize = SaveInventorySize;
	Umir->Inventory->Gold = SaveGold;
}

void UTakeMeHomeGameInstance::ResetUmir()
{
	UE_LOG(LogTemp, Warning, TEXT("resetUmir"));
	auto Umir = Cast<AUmir>(GetWorld()->GetFirstPlayerController()->GetCharacter());
	if (!ensure(Umir)) return;

	// Action bar
	SaveDefensiveSpellBound = EDefensiveSpell::DS_Spirit_Walk;
	SaveOffensiveSpell1Bound = EOffensiveSpell::OS_None;
	SaveOffensiveSpell2Bound = EOffensiveSpell::OS_None;
	SaveOffensiveSpell3Bound = EOffensiveSpell::OS_None;
	SavePotionBound = EPotion::P_None;

	// Abilities
	SaveOffensiveSpells.Empty();
	SaveDefensiveSpells.Empty();
	SaveDefensiveSpells.Add(*DefensiveSpells.Find(EDefensiveSpell::DS_Spirit_Walk));
	SaveDefensiveSpells.Add(*DefensiveSpells.Find(EDefensiveSpell::DS_Counter_Strike));
	Umir->Abilities->bNewDefensive = true;
	SavePhysicalAttacks.Empty();
	SavePotions.Empty();

	// Inventory
	SaveItems.Empty();
	SaveInventorySize = 100;
	SaveGold = 10;

	LoadUmir();
}
