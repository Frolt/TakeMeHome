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
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool AddItems(EItem Item, int32 Quantity);
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool AddItemsAtIndex(EItem Item, int32 Quantity, int32 Index);
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool RemoveItems(EItem Item, int32 Quantity);
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool RemoveItemsAtIndex(EItem Item, int32 Quantity, int32 Index);

	// Sorting
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void SortByName();
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void SortByQuantity();
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void SortByQuality();

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
