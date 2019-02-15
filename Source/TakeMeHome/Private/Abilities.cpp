// Fill out your copyright notice in the Description page of Project Settings.

#include "Abilities.h"
#include "TakeMeHomeGameInstance.h"
#include "Engine/World.h"
#include "Umir.h"


UAbilities::UAbilities()
{
	PrimaryComponentTick.bCanEverTick = false;


}

void UAbilities::BeginPlay()
{
	Super::BeginPlay();

	// Find game instance
	GameInstance = Cast<UTakeMeHomeGameInstance>(GetWorld()->GetGameInstance());
}

void UAbilities::AddDefensive(EDefensiveSpell DefensiveSpell)
{
	DefensiveSpells.Add(*GameInstance->DefensiveSpells.Find(DefensiveSpell));
}

void UAbilities::AddOffensive(EOffensiveSpell OffensiveSpell)
{
	OffensiveSpells.Add(*GameInstance->OffensiveSpells.Find(OffensiveSpell));
}

void UAbilities::AddPhysical(EPhysicalAttack PhysicalAttack)
{
	PhysicalAttacks.Add(*GameInstance->PhysicalAttacks.Find(PhysicalAttack));
}

bool UAbilities::AddPotion(EPotion Key)
{
	int32 Index = Potions.IndexOfByPredicate([Key](const FPotion &A) {
		return A.Key == Key;
	});

	if (Index == INDEX_NONE)
	{
		Potions.Add(*GameInstance->Potions.Find(Key));
		return true;
	}
	else
	{
		if (Potions[Index].Quantity < Potions[Index].MaxQuantity)
		{
			Potions[Index].Quantity++;
			return true;
		}
		else
		{
			if (auto Umir = Cast<AUmir>(GetOwner()))
			{
				Umir->PotionsAreFull();
			}
			return false;
		}
	}
}

bool UAbilities::RemoveDefensive(EDefensiveSpell Key)
{
	auto Index = DefensiveSpells.IndexOfByPredicate([Key](const FDefensiveSpell &Element) {
		return Element.Key == Key;
	});
	if (Index != INDEX_NONE)
	{
		DefensiveSpells.RemoveAt(Index);
		return true;
	}
	else
	{
		return false;
	}
}

bool UAbilities::RemoveOffensive(EOffensiveSpell Key)
{
	auto Index = OffensiveSpells.IndexOfByPredicate([Key](const FOffensiveSpell &Element) {
		return Element.Key == Key;
	});
	if (Index != INDEX_NONE)
	{
		OffensiveSpells.RemoveAt(Index);
		return true;
	}
	else
	{
		return false;
	}
}

bool UAbilities::RemovePhysical(EPhysicalAttack Key)
{
	auto Index = PhysicalAttacks.IndexOfByPredicate([Key](const FPhysicalAttack &Element) {
		return Element.Key == Key;
	});
	if (Index != INDEX_NONE)
	{
		PhysicalAttacks.RemoveAt(Index);
		return true;
	}
	else
	{
		return false;
	}
}

bool UAbilities::RemovePotion(EPotion Key)
{
	int32 Index = Potions.IndexOfByPredicate([Key](const FPotion &A) {
		return A.Key == Key;
	});

	if (Index == INDEX_NONE)
	{
		return false;
	}
	else if (Potions[Index].Quantity > 1)
	{
		Potions[Index].Quantity--;
		return true;
	}
	else
	{
		Potions.RemoveAt(Index);
		return true;
	}
}

FPotion UAbilities::GetPotion(EPotion Key) const
{
	auto *PotionPtr = Potions.FindByPredicate([Key](const FPotion &A) {
		return A.Key == Key;
	});

	if (auto lol = PotionPtr)
	{
		return *PotionPtr;
	}
	else
	{
		return FPotion();
	}
}
