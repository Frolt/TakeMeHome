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
	PhysicalAttacks.Add(*GameInstance->NormalAttacks.Find(PhysicalAttack));
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

bool UAbilities::RemovePotion(EPotion Key)
{
	int32 Index = Potions.IndexOfByPredicate([Key](const FPotion &A) {
		return A.Key == Key;
	});

	if (Index == INDEX_NONE)
	{
		return false;
	}
	else
	{
		if (Potions[Index].Quantity >= 2)
		{
			Potions[Index].Quantity--;
		}
		else
		{
			Potions.RemoveAt(Index);
		}
		return true;
	}
}

FPotion UAbilities::GetPotion(EPotion Key) const
{
	if (Key != EPotion::P_None)
	{
		return *Potions.FindByPredicate([Key](const FPotion &A) {
			return A.Key == Key;
		});
	}
	else
	{
		return FPotion();
	}
}
