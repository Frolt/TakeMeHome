// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TakeMeHomeStructs.h"
#include "Inventory.generated.h"

class UTakeMeHomeGameInstance;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TAKEMEHOME_API UInventory : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventory();
	virtual void BeginPlay() override;

	// Add / Remove
	UFUNCTION(BlueprintCallable, Category = "Utilities")
	bool AddItems(EItem Item, int32 Quantity);
	UFUNCTION(BlueprintCallable, Category = "Utilities")
	bool AddItemsAtIndex(EItem Item, int32 Quantity, int32 Index);
	UFUNCTION(BlueprintCallable, Category = "Utilities")
	bool RemoveItems(EItem Item, int32 Quantity);
	UFUNCTION(BlueprintCallable, Category = "Utilities")
	bool RemoveItemsAtIndex(EItem Item, int32 Quantity, int32 Index);

	// Sorting
	UFUNCTION(BlueprintCallable, Category = "Sorting")
	void SortByName();
	UFUNCTION(BlueprintCallable, Category = "Sorting")
	void SortByQuantity();
	UFUNCTION(BlueprintCallable, Category = "Sorting")
	void SortByQuality();
	UFUNCTION(BlueprintCallable, Category = "Sorting")
	void SortByValue();

	// Game instance ref (Safe to use since game instance are never destroyed)
	UPROPERTY(BlueprintReadOnly, Category = "Game Instance")
	UTakeMeHomeGameInstance *GameInstance = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Settings")
	TArray<FItem> Items;
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	int32 InventorySize = 100;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gold")
	int32 Gold = 10;

};
