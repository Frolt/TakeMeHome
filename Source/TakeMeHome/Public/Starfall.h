// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpellBase.h"
#include "Starfall.generated.h"

class AStaticMeshActor;

/**
 * 
 */
UCLASS()
class TAKEMEHOME_API AStarfall : public ASpellBase
{
	GENERATED_BODY()
	
public:
	AStarfall();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void SpawnProjectile();

private:
	UPROPERTY(EditAnywhere, Category = "Spell setup")
	TSubclassOf<AStaticMeshActor> TestBP;
	
private:
	UPROPERTY(EditAnywhere, Category = "Spell settings")
	int NumOfProjectiles = 5;
	UPROPERTY(EditAnywhere, Category = "Spell settings")
	float SpawnHeight = 500.0f;
	UPROPERTY(EditAnywhere, Category = "Spell settings")
	float SpawnDelta = 500.0f;
	UPROPERTY(EditAnywhere, Category = "Spell settings")
	float FirstSpawnDelay = 0.0f;
	UPROPERTY(EditAnywhere, Category = "Spell settings")
	float SpawnTimeInterval = 0.2f;
	int Index = 0.0f;
	TArray<FVector> SpawnLocations;
};
