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

bool UInventory::AddItems(EItem Item, int32 Quantity)
{
	// Check if inventory has space for the items
	// ------------------------------------------------------------------------------------
	int32 MaxItemsCanHold = 0;
	int32 NumOfEmpty = InventorySize - Items.Num();
	int32 MaxQuantity = GameInstance->Items.Find(Item)->MaxQuantity;
	for (auto &Element : Items)
	{
		if (Element.Key == Item)
			MaxItemsCanHold += Element.MaxQuantity - Element.Quantity;
	}
	MaxItemsCanHold += NumOfEmpty * MaxQuantity;
	if (Quantity > MaxItemsCanHold)
		return false;
	else if (Quantity <= 0)
		return true;
	// ------------------------------------------------------------------------------------

	// Fill up previous item stacks
	// ------------------------------------------------------------------------------------
	for (auto &Element : Items)
	{
		if (Element.Key == Item)
		{
			int32 DeltaSpace = Element.MaxQuantity - Element.Quantity;
			if (Quantity <= DeltaSpace)
			{
				Element.Quantity += Quantity;
				return true;
			}
			else
			{
				Element.Quantity += DeltaSpace;
				Quantity -= DeltaSpace;
			}
		}
	}
	// ------------------------------------------------------------------------------------

	// Add new items
	// ------------------------------------------------------------------------------------
	for (int32 i = 0; i < NumOfEmpty; i++)
	{
		if (Quantity <= MaxQuantity)
		{
			int32 NewItem = Items.Add(*GameInstance->Items.Find(Item));
			Items[NewItem].Quantity = Quantity;
			return true;
		}
		else
		{
			int32 NewItem = Items.Add(*GameInstance->Items.Find(Item));
			Items[NewItem].Quantity = MaxQuantity;
			Quantity -= MaxQuantity;
		}
	}
	// ------------------------------------------------------------------------------------

	return true;
}

bool UInventory::AddItemsAtIndex(EItem Item, int32 Quantity, int32 Index)
{
	// TODO

	// check if index is full
	// fill it up as much as possible
	// call AddItems() to add rest

	return true;
}

bool UInventory::RemoveItems(EItem Item, int32 Quantity)
{
	// Check if inventory contains the amount of items
	// ------------------------------------------------------------------------------------
	int32 NumOfItems = 0;
	for (auto &Element : Items)
	{
		if (Element.Key == Item)
			NumOfItems += Element.Quantity;
	}
	if (Quantity > NumOfItems)
		return false;
	else if (Quantity <= 0)
		return true;
	// ------------------------------------------------------------------------------------

	// Remove items
	// ------------------------------------------------------------------------------------
	for (int32 i = 0; i < Items.Num(); i++)
	{
		if (Items[i].Key == Item)
		{
			if (Quantity < Items[i].Quantity)
			{
				Items[i].Quantity -= Quantity;
				return true;
			}
			else
			{
				Quantity -= Items[i].Quantity;
				Items.RemoveAt(i--);
			}
		}
	}
	// ------------------------------------------------------------------------------------

	return true;
}

bool UInventory::RemoveItemsAtIndex(EItem Item, int32 Quantity, int32 Index)
{
	// TODO

	// check if index is not full
	// remove as much as possible
	// replace with "Empty struct"
	// call RemoveItems() to remove rest

	return true;
}

void UInventory::SortByName()
{
	Items.Sort([](const FItem& A, const FItem& B) {
		return A.Name.ToString() < B.Name.ToString();
	});
}

void UInventory::SortByQuantity()
{
	Items.Sort([](const FItem& A, const FItem& B) {
		return A.Quantity > B.Quantity;
	});
}

void UInventory::SortByQuality()
{
	Items.Sort([](const FItem& A, const FItem& B) {
		return A.Quality > B.Quality;
	});
}

void UInventory::SortByValue()
{

}
