// Fill out your copyright notice in the Description page of Project Settings.

#include "Inventory.h"
#include "TakeMeHomeEnums.h"
#include "Engine/World.h"
#include "TakeMeHomeGameInstance.h"


UInventory::UInventory()
{
	PrimaryComponentTick.bCanEverTick = false;


}


void UInventory::BeginPlay()
{
	Super::BeginPlay();

	GameInstance = Cast<UTakeMeHomeGameInstance>(GetWorld()->GetGameInstance());
}

bool UInventory::AddItem(EItem Item)
{
	if (Items.Num() < InventorySize)
	{
		Items.Add(*GameInstance->Items.Find(Item));
		return true;
	}
	return false;
}

bool UInventory::RemoveSingleItem(EItem Item)
{
	//Items.RemoveSingle(*GameInstance->Items.Find(Item));

	int32 Index = Items.IndexOfByPredicate([Item](const FItem& A) {
		return A.Key == Item;
	});
	if (Index != INDEX_NONE)
	{
		Items.RemoveAt(Index);
		return true;
	}
	return false;
}

bool UInventory::RemoveAllItemsOfType(EItem Item)
{
	// TODO
	//Items.Remove(*GameInstance->Items.Find(Item));
	return true;
}

void UInventory::SortByName()
{
	Items.Sort([](const FItem& A, const FItem& B) {
		return A.Name > B.Name;
	});
}
