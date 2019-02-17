// Fill out your copyright notice in the Description page of Project Settings.

#include "Starfall.h"
#include "Engine/World.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/Public/TimerManager.h"
#include "StarfallProjectile.h"
#include "TakeMeHomeGameInstance.h"
#include "TakeMeHomeEnums.h"
#include "Kismet/GameplayStatics.h"


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
	if (!ensure(StarfallProjectileBP)) return;
	if (!ensure(SpellMarker)) return;

	if (Index + 1 < SpawnLocations.Num())
	{
		FTransform SpawnTransform(GetActorRotation() + ProjectileRotation, SpawnLocations[Index++]);
		auto Projectile = GetWorld()->SpawnActorDeferred<AStarfallProjectile>(StarfallProjectileBP, SpawnTransform);
		Projectile->Force = ProjectileSpeed;
		Projectile->Damage = Damage;
		Projectile->AbilityOwner = AbilityOwner;
		Projectile->FinishSpawning(SpawnTransform);

		if (Index == SpawnLocations.Num())
		{
			// Done spawning projectiles
			GetWorldTimerManager().ClearAllTimersForObject(this);
			Destroy();
		}

		// Spawn spell marker particle
		auto EndTrace = SpawnLocations[Index - 1];
		EndTrace.Z -= 100000.0f;
		FHitResult HitResult;
		GetWorld()->LineTraceSingleByChannel(HitResult, SpawnLocations[Index - 1], EndTrace, ECC_WorldStatic);
		auto SpawnedParticle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SpellMarker, HitResult.Location);
	}
}
