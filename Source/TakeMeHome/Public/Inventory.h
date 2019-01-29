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

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool AddItem(EItem Item);
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool RemoveSingleItem(EItem Item);
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool RemoveAllItemsOfType(EItem Item);
	UFUNCTION(BlueprintCallable, Category = "Abilities")
	void SortByName();

	// Game instance ref (Safe to use since game instance are never destroyed)
	UTakeMeHomeGameInstance *GameInstance = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Settings")
	TArray<FItem> Items;
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	int32 InventorySize = 100;

};
