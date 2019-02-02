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
	// TODO
	return true;
}

bool UInventory::AddItemsAtIndex(EItem Item, int32 Quantity, int32 Index)
{
	// TODO
	return true;
}

bool UInventory::RemoveItems(EItem Item, int32 Quantity)
{
	// TODO
	return true;
}

bool UInventory::RemoveItemsAtIndex(EItem Item, int32 Quantity, int32 Index)
{
	// TODO
	return true;
}

void UInventory::SortByName()
{
	Items.Sort([](const FItem& A, const FItem& B) {
		return A.Name.ToString() > B.Name.ToString();
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
