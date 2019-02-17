// Fill out your copyright notice in the Description page of Project Settings.

#include "TakeMeHomeGameInstance.h"


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
