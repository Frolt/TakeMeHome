// Fill out your copyright notice in the Description page of Project Settings.

#include "Starfall.h"
#include "Engine/World.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/Public/TimerManager.h"
#include "StarfallProjectile.h"


AStarfall::AStarfall()
{
	PrimaryActorTick.bCanEverTick = false;


}

void AStarfall::BeginPlay()
{
	Super::BeginPlay();

	// Find first spawn position above the player
	auto InitLocation = GetActorLocation();
	InitLocation.Z += SpawnHeight;
	auto ForwardVector = GetActorForwardVector();
	SpawnLocations.SetNum(NumOfProjectiles);

	for (int32 i = 0; i < NumOfProjectiles; i++)
	{
		SpawnLocations[i] = InitLocation + (SpawnDelta * ForwardVector * i);
	}

	// Fire timer based on SpawnTimeInterval
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AStarfall::SpawnProjectile, SpawnTimeInterval, true, FirstSpawnDelay);
}

void AStarfall::SpawnProjectile()
{
	if (Index + 1 < SpawnLocations.Num())
	{
		auto Projectile = GetWorld()->SpawnActor<AStarfallProjectile>(StarfallProjectileBP, SpawnLocations[Index++], GetActorRotation() + ProjectileRotation);
		Projectile->Force = ProjectileSpeed;

		if (Index == SpawnLocations.Num())
		{
			// Done spawning projectiles
			GetWorldTimerManager().ClearAllTimersForObject(this);
			Destroy();
		}
	}
}
