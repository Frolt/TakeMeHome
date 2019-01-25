// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpellBase.h"
#include "Starfall.generated.h"

class AStaticMeshActor;
class AStarfallProjectile;

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

	void SpawnProjectile();

private:
	UPROPERTY(EditAnywhere, Category = "Spell setup")
	TSubclassOf<AStarfallProjectile> StarfallProjectileBP;
	
private:
	UPROPERTY(EditAnywhere, Category = "Spell settings")
	int32 NumOfProjectiles = 5;
	UPROPERTY(EditAnywhere, Category = "Spell settings")
	float SpawnHeight = 500.0f;
	UPROPERTY(EditAnywhere, Category = "Spell settings")
	float SpawnDelta = 500.0f;
	UPROPERTY(EditAnywhere, Category = "Spell settings")
	float FirstSpawnDelay = 0.0f;
	UPROPERTY(EditAnywhere, Category = "Spell settings")
	float SpawnTimeInterval = 0.2f;
	UPROPERTY(EditAnywhere, Category = "Spell settings")
	FRotator ProjectileRotation = FRotator(-80.0f, 0.0f, 0.0f);
	UPROPERTY(EditAnywhere, Category = "Spell settings")
	float ProjectileSpeed = 1000.0f;

	int32 Index = 0.0f;
	TArray<FVector> SpawnLocations;
};
