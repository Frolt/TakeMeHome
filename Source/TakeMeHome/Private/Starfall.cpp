// Fill out your copyright notice in the Description page of Project Settings.

#include "Starfall.h"
#include "Engine/World.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/Public/TimerManager.h"
#include "StarfallProjectile.h"
#include "TakeMeHomeGameInstance.h"
#include "TakeMeHomeEnums.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"


AStarfall::AStarfall()
{
	PrimaryActorTick.bCanEverTick = false;


}

void AStarfall::BeginPlay()
{
	Super::BeginPlay();

	// Find spawn locations
	auto InitLocation = GetActorLocation();
	InitLocation.Z += SpawnHeight;
	auto ForwardVector = GetActorForwardVector();
	SpawnLocations.SetNum(NumOfProjectiles);
	for (int32 i = 0; i < NumOfProjectiles; i++)
	{
		SpawnLocations[i] = InitLocation + (SpawnDelta * ForwardVector * i);
	}

	// Find landing locations
	LandingLocations.SetNum(NumOfProjectiles);
	for (int32 i = 0; i < NumOfProjectiles; i++)
	{
		// Linetrace down with offset
		auto StartPos = SpawnLocations[i] + (ForwardVector * LandingForwardOffset);
		auto EndPos = StartPos + (-FVector::UpVector * 100000.0f);
		FHitResult HitResult;
		GetWorld()->LineTraceSingleByChannel(HitResult, StartPos, EndPos, ECC_WorldStatic);
		LandingLocations[i] = HitResult.Location;
	}

	// Add spawn location offset
	bool bAlterate = true;
	for (auto &element : SpawnLocations)
	{
		if (bAlterate)
		{
			element += GetActorRightVector() * SpawnSideOffset;
			bAlterate = false;
		}
		else
		{
			element = element + (-GetActorRightVector() * SpawnSideOffset);
			bAlterate = true;
		}
	}

	// Find the projectiles launch velocity
	LaunchVelocities.SetNum(NumOfProjectiles);
	for (int32 i = 0; i < NumOfProjectiles; i++)
	{
		UGameplayStatics::SuggestProjectileVelocity(
			this, 
			LaunchVelocities[i], 
			SpawnLocations[i], 
			LandingLocations[i], 
			ProjectileSpeed, 
			false, 
			0.0f, 
			0.0f, 
			ESuggestProjVelocityTraceOption::DoNotTrace
		);
	}

	// Fire timer based on SpawnTimeInterval
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AStarfall::SpawnProjectile, SpawnTimeInterval, true, FirstSpawnDelay);
}

void AStarfall::SpawnProjectile()
{
	if (!ensure(StarfallProjectileBP)) return;
	if (!ensure(SpellMarker)) return;

	if (Index < SpawnLocations.Num())
	{
		// Spawn projectile
		auto Rot = GetActorRotation();
		Rot.Pitch -= 180.0f;
		FTransform SpawnTransform(FRotator::ZeroRotator, SpawnLocations[Index]);
		auto Projectile = GetWorld()->SpawnActorDeferred<AStarfallProjectile>(StarfallProjectileBP, SpawnTransform);
		Projectile->AbilityOwner = AbilityOwner;
		Projectile->Damage = Damage;
		Projectile->ElementType = ElementType;
		Projectile->DamageType = DamageType;
		Projectile->LaunchVelocity = LaunchVelocities[Index];
		Projectile->FinishSpawning(SpawnTransform);

		// Spawn spell marker particle
		auto SpawnedParticle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SpellMarker, LandingLocations[Index]);
		FTimerDelegate TimerDel;
		FTimerHandle TimerHandle;
		//TimerDel.BindUFunction(this, FName("DestroySpellMark"), 1, 2);
		TimerDel.BindLambda([SpawnedParticle]() {
			SpawnedParticle->DestroyComponent();
		});
		GetWorldTimerManager().SetTimer(TimerHandle, TimerDel, 2.0f, false);
		Index++;
	}
	else
	{
		// Done spawning projectiles
		GetWorldTimerManager().ClearAllTimersForObject(this);
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, this, &AStarfall::DestroyEvent, 5.0f, false);
	}
}

void AStarfall::DestroyEvent()
{
	Destroy();
}
