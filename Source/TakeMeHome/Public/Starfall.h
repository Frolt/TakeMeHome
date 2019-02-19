// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OffensiveSpellBase.h"
#include "Starfall.generated.h"

class AStaticMeshActor;
class AStarfallProjectile;
class UParticleSystem;
class UParticleSystemComponent;

/**
 * 
 */
UCLASS()
class TAKEMEHOME_API AStarfall : public AOffensiveSpellBase
{
	GENERATED_BODY()
	
public:
	AStarfall();
	virtual void BeginPlay() override;

	void SpawnProjectile();
	void DestroyEvent();

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
	float LandingForwardOffset = 200.0f;
	UPROPERTY(EditAnywhere, Category = "Spell settings")
	float SpawnSideOffset = 200.0f;
	UPROPERTY(EditAnywhere, Category = "Spell settings")
	float SpawnTimeInterval = 0.2f;
	UPROPERTY(EditAnywhere, Category = "Spell settings")
	float ProjectileSpeed = 1000.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Spell settings")
	UParticleSystem *SpellMarker = nullptr;

	int32 Index = 0.0f;
	TArray<FVector> SpawnLocations;
	TArray<FVector> LandingLocations;
	TArray<FVector> LaunchVelocities;
};
