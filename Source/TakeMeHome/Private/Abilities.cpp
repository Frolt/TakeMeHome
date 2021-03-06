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
	bNewDefensive = true;
	DefensiveSpells.Add(*GameInstance->GetDefensiveSpell(DefensiveSpell));
}

void UAbilities::AddOffensive(EOffensiveSpell OffensiveSpell)
{
	bNewOffensive = true;
	OffensiveSpells.Add(*GameInstance->GetOffensiveSpell(OffensiveSpell));
}

void UAbilities::AddPhysical(EPhysicalAttack PhysicalAttack)
{
	PhysicalAttacks.Add(*GameInstance->GetPhysicalAttack(PhysicalAttack));
}

bool UAbilities::AddPotion(EPotion Potion, int32 Num /*= 1*/)
{
	bNewPotion = true;

	int32 Index = Potions.IndexOfByPredicate([Potion](const FPotion &A) {
		return A.Key == Potion;
	});

	if (Index == INDEX_NONE)
	{
		Potions.Add(*GameInstance->GetPotion(Potion));
		if (Num <= Potions.Last().MaxQuantity)
		{
			Potions.Last().Quantity = Num;
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		auto SpaceLeft = Potions[Index].MaxQuantity - Potions[Index].Quantity;
		if (Num <= SpaceLeft)
		{
			Potions[Index].Quantity += Num;
			return true;
		}
		else
		{
			// Hud warning if Umir
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

int32 UAbilities::GetPotionsLeft(EPotion Key) const
{
	auto PotionPtr = Potions.FindByPredicate([Key](const FPotion &A) {
		return A.Key == Key;
	});

	if (PotionPtr)
	{
		return PotionPtr->Quantity;
	}
	else
	{
		return 0;
	}
}
